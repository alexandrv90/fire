#include "sim/FireSimulation.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {
constexpr std::uint32_t FALLBACK_SEED = 0xA341316Cu;

constexpr int PALETTE_SHIFT = 6;
constexpr std::uint32_t MAXIMUM_HEAT = (1u << (8 + PALETTE_SHIFT)) - 1u;

constexpr std::size_t PAD_COLUMNS = 1;
constexpr std::size_t SOURCE_ROWS = 2;

// A centre-heavy kernel preserves narrow tongues while moving heat upward.
constexpr std::uint32_t SIDE_WEIGHT = 1;
constexpr std::uint32_t CENTRE_WEIGHT = 10;
constexpr std::uint32_t LIFT_WEIGHT = 4;
constexpr int KERNEL_SHIFT = 4; // 2*1 + 10 + 4 == 16

constexpr std::size_t COOLING_ROWS = 1024;
constexpr int COOLING_SHIFT = 12;
constexpr std::uint32_t COOLING_UNIT = 1u << COOLING_SHIFT;
constexpr std::uint32_t MAXIMUM_COOLING_AMPLITUDE = 1u << 15;

// Vertically stretched octaves form the tongues and break up their edges.
constexpr std::size_t COARSE_SPACING_X = 8;
constexpr std::size_t COARSE_SPACING_Y = 32;
constexpr double COARSE_WEIGHT = 0.65;
constexpr std::size_t FINE_SPACING_X = 2;
constexpr std::size_t FINE_SPACING_Y = 8;
constexpr double FINE_WEIGHT = 0.35;

// Cooling maps the parameter range to resolution-independent flame reach.
constexpr double MAXIMUM_REACH = 1.1;
constexpr double MINIMUM_REACH = 0.15;
constexpr double AMPLITUDE_SCALE = 2.72;

[[nodiscard]] std::size_t roundUp(const std::size_t value, const std::size_t multiple) noexcept {
    return (value + multiple - 1) / multiple * multiple;
}

[[nodiscard]] bool productFits(const std::size_t left, const std::size_t right, const std::size_t maximum) noexcept {
    return left == 0 || right <= maximum / left;
}
} // namespace

FireSimulation::Cell* FireSimulation::mutableRow(const std::size_t y) noexcept {
    return heatField.data() + y * fieldStride + PAD_COLUMNS;
}

const FireSimulation::Cell* FireSimulation::row(const std::size_t y) const noexcept {
    return heatField.data() + y * fieldStride + PAD_COLUMNS;
}

FireSimulation::FireSimulation(const Dimensions dimensions, const std::uint32_t randomSeed)
    : simulationDimensions(dimensions), randomState(randomSeed == 0 ? FALLBACK_SEED : randomSeed) {
    if (simulationDimensions.width < 2 || simulationDimensions.height < 2) {
        throw std::invalid_argument("Fire dimensions must both be at least 2");
    }

    constexpr std::size_t MAXIMUM_SIZE = std::numeric_limits<std::size_t>::max();
    if (!simulationDimensions.hasRepresentableArea() || simulationDimensions.area() > heatMap.max_size() ||
        simulationDimensions.width > MAXIMUM_SIZE - (COARSE_SPACING_X - 1)) {
        throw std::length_error("Fire dimensions are too large");
    }

    // Complete lattice cells keep the wrapped cooling noise seamless.
    fieldWidth = roundUp(simulationDimensions.width, COARSE_SPACING_X);
    if (fieldWidth > MAXIMUM_SIZE - 2 * PAD_COLUMNS || simulationDimensions.height > MAXIMUM_SIZE - SOURCE_ROWS) {
        throw std::length_error("Fire dimensions are too large");
    }

    fieldStride = fieldWidth + 2 * PAD_COLUMNS;
    const std::size_t fieldHeight = simulationDimensions.height + SOURCE_ROWS;
    if (!productFits(fieldStride, fieldHeight, heatField.max_size()) ||
        !productFits(fieldWidth, COOLING_ROWS, coolingMap.max_size())) {
        throw std::length_error("Fire dimensions are too large");
    }

    heatField.resize(fieldStride * fieldHeight);
    heatMap.resize(simulationDimensions.area());
    buildCoolingMap();
    reset();
}

void FireSimulation::tick() noexcept {
    wrapEdges();

    const std::uint32_t amplitude = coolingAmplitude;
    for (std::size_t y = 0; y < simulationDimensions.height; ++y) {
        const Cell* const below = row(y + 1);
        const Cell* const belowLeft = below - 1;
        const Cell* const belowRight = below + 1;
        const Cell* const belowTwo = row(y + 2);
        Cell* const destination = mutableRow(y);

        const std::size_t mapRow = (y + coolingScroll) & (COOLING_ROWS - 1);
        const std::uint16_t* const cool = coolingMap.data() + mapRow * fieldWidth;

        for (std::size_t x = 0; x < fieldWidth; ++x) {
            const std::uint32_t diffused =
                (SIDE_WEIGHT * (belowLeft[x] + belowRight[x]) + CENTRE_WEIGHT * below[x] + LIFT_WEIGHT * belowTwo[x]) >>
                KERNEL_SHIFT;
            const std::uint32_t lost = (cool[x] * amplitude) >> COOLING_SHIFT;
            destination[x] = static_cast<Cell>(diffused > lost ? diffused - lost : 0u);
        }
    }

    // Moving the map with the heat keeps flame tongues coherent.
    coolingScroll = (coolingScroll + 1) & (COOLING_ROWS - 1);
    heatMapStale = true;
}

void FireSimulation::reset() noexcept {
    std::fill(heatField.begin(), heatField.end(), Cell{0});
    coolingScroll = 0;
    applyParameters();
    heatMapStale = true;
}

HeatFrame FireSimulation::heat() const noexcept {
    if (heatMapStale) {
        refreshHeatMap();
    }
    return {heatMap, simulationDimensions};
}

void FireSimulation::setParameters(const FireParameters& parameters) noexcept {
    simulationParameters = parameters;
    applyParameters();
}

std::uint32_t FireSimulation::nextRandom() noexcept {
    // Xorshift32 is compact, deterministic and adequate for visual noise.
    randomState ^= randomState << 13u;
    randomState ^= randomState >> 17u;
    randomState ^= randomState << 5u;
    return randomState;
}

void FireSimulation::applyParameters() noexcept {
    // Solve the per-row cooling rate from the desired flame reach. Geometric
    // interpolation gives useful control across the full parameter range.
    constexpr double COOLING_SPAN = FireParameters::MAXIMUM_COOLING - FireParameters::MINIMUM_COOLING;
    const double cooling = (simulationParameters.cooling() - FireParameters::MINIMUM_COOLING) / COOLING_SPAN;
    const double reach = MAXIMUM_REACH * std::pow(MINIMUM_REACH / MAXIMUM_REACH, cooling);
    const double amplitude =
        AMPLITUDE_SCALE * MAXIMUM_HEAT / (reach * static_cast<double>(simulationDimensions.height));
    coolingAmplitude =
        static_cast<std::uint32_t>(std::clamp<double>(std::lround(amplitude), 1.0, MAXIMUM_COOLING_AMPLITUDE));

    // A flat source leaves all flame structure to the cooling map.
    const auto sourceHeat = static_cast<Cell>(MAXIMUM_HEAT * simulationParameters.sourceHeat() / 255u);
    std::fill_n(mutableRow(simulationDimensions.height), fieldWidth, sourceHeat);
    std::fill_n(mutableRow(simulationDimensions.height + 1), fieldWidth, sourceHeat);
}

void FireSimulation::buildCoolingMap() {
    coolingMap.assign(fieldWidth * COOLING_ROWS, std::uint16_t{0});
    addNoiseOctave(COARSE_SPACING_X, COARSE_SPACING_Y, COARSE_WEIGHT);
    addNoiseOctave(FINE_SPACING_X, FINE_SPACING_Y, FINE_WEIGHT);
}

void FireSimulation::addNoiseOctave(const std::size_t spacingX, const std::size_t spacingY, const double weight) {
    // Bilinearly interpolated value noise wraps in both axes.
    const std::size_t latticeWidth = fieldWidth / spacingX;
    const std::size_t latticeHeight = COOLING_ROWS / spacingY;

    std::vector<double> lattice(latticeWidth * latticeHeight);
    for (double& value : lattice) {
        value = static_cast<double>(nextRandom() >> 8u) / 16777216.0;
    }

    const double gain = weight * COOLING_UNIT;
    for (std::size_t y = 0; y < COOLING_ROWS; ++y) {
        const std::size_t gridY = y / spacingY;
        const std::size_t gridYNext = (gridY + 1) % latticeHeight;
        const double weightY = static_cast<double>(y % spacingY) / static_cast<double>(spacingY);

        const double* const nearRow = lattice.data() + gridY * latticeWidth;
        const double* const farRow = lattice.data() + gridYNext * latticeWidth;
        std::uint16_t* const destination = coolingMap.data() + y * fieldWidth;

        for (std::size_t x = 0; x < fieldWidth; ++x) {
            const std::size_t gridX = x / spacingX;
            const std::size_t gridXNext = (gridX + 1) % latticeWidth;
            const double weightX = static_cast<double>(x % spacingX) / static_cast<double>(spacingX);

            const double a = nearRow[gridX];
            const double b = nearRow[gridXNext];
            const double c = farRow[gridX];
            const double d = farRow[gridXNext];
            const double value = std::lerp(std::lerp(a, b, weightX), std::lerp(c, d, weightX), weightY);

            destination[x] = static_cast<std::uint16_t>(destination[x] + static_cast<std::uint16_t>(value * gain));
        }
    }
}

void FireSimulation::wrapEdges() noexcept {
    static_assert(PAD_COLUMNS < COARSE_SPACING_X);

    for (std::size_t y = 0; y < simulationDimensions.height + SOURCE_ROWS; ++y) {
        Cell* const cells = mutableRow(y);
        cells[-1] = cells[fieldWidth - 1];
        cells[fieldWidth] = cells[0];
    }
}

void FireSimulation::refreshHeatMap() const noexcept {
    for (std::size_t y = 0; y < simulationDimensions.height; ++y) {
        const Cell* const cells = row(y);
        std::uint8_t* const destination = heatMap.data() + y * simulationDimensions.width;
        for (std::size_t x = 0; x < simulationDimensions.width; ++x) {
            destination[x] = static_cast<std::uint8_t>(cells[x] >> PALETTE_SHIFT);
        }
    }
    heatMapStale = false;
}
