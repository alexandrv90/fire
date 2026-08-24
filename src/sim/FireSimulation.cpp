#include "sim/FireSimulation.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace {
constexpr std::uint32_t FALLBACK_SEED = 0xA341316Cu;
constexpr std::uint32_t SIDE_WEIGHT = 1;
constexpr std::uint32_t NEAR_CENTER_WEIGHT = 10;
constexpr std::uint32_t FAR_CENTER_WEIGHT = 4;
constexpr std::uint32_t WEIGHT_DIVISOR = 16;

static_assert(2 * SIDE_WEIGHT + NEAR_CENTER_WEIGHT + FAR_CENTER_WEIGHT == WEIGHT_DIVISOR);
} // namespace

FireSimulation::FireSimulation(const std::size_t width, const std::size_t height, const std::uint32_t randomSeed)
    : simulationWidth(width), simulationHeight(height), initialSeed(randomSeed == 0 ? FALLBACK_SEED : randomSeed),
      randomState(initialSeed) {
    if (simulationWidth < 2 || simulationHeight < 2) {
        throw std::invalid_argument("Fire dimensions must both be at least 2");
    }
    if (simulationWidth > static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) ||
        simulationHeight > std::numeric_limits<std::size_t>::max() / simulationWidth) {
        throw std::length_error("Fire dimensions are too large");
    }

    heatMap.resize(simulationWidth * simulationHeight);
    reset();
}

void FireSimulation::tick() noexcept {
    auto* const cells = heatMap.data();
    const std::uint8_t cooling = simulationParameters.cooling();

    // Propagate from lower rows into the row above. Updating top-to-bottom is
    // intentional: every source row still contains the previous frame's heat.
    for (std::size_t y = 0; y + 1 < simulationHeight; ++y) {
        const std::size_t destinationOffset = y * simulationWidth;
        const std::size_t nearSourceOffset = destinationOffset + simulationWidth;
        const std::size_t farSourceY = std::min(y + 2, simulationHeight - 1);
        const std::size_t farSourceOffset = farSourceY * simulationWidth;

        for (std::size_t x = 0; x < simulationWidth; ++x) {
            const std::size_t leftX = x == 0 ? x : x - 1;
            const std::size_t rightX = x + 1 == simulationWidth ? x : x + 1;
            const std::uint32_t weightedHeat =
                SIDE_WEIGHT * cells[nearSourceOffset + leftX] + NEAR_CENTER_WEIGHT * cells[nearSourceOffset + x] +
                SIDE_WEIGHT * cells[nearSourceOffset + rightX] + FAR_CENTER_WEIGHT * cells[farSourceOffset + x];
            const auto averagedHeat = static_cast<std::uint8_t>(weightedHeat / WEIGHT_DIVISOR);

            cells[destinationOffset + x] =
                averagedHeat > cooling ? static_cast<std::uint8_t>(averagedHeat - cooling) : std::uint8_t{0};
        }
    }

    updateFuelRow();
}

void FireSimulation::reset() noexcept {
    std::fill(heatMap.begin(), heatMap.end(), std::uint8_t{0});
    randomState = initialSeed;
    updateFuelRow();
}

std::uint32_t FireSimulation::nextRandom() noexcept {
    // Xorshift32 is compact, deterministic and adequate for visual noise.
    randomState ^= randomState << 13u;
    randomState ^= randomState >> 17u;
    randomState ^= randomState << 5u;
    return randomState;
}

void FireSimulation::updateFuelRow() noexcept {
    const std::size_t rowOffset = (simulationHeight - 1) * simulationWidth;
    const std::uint8_t sourceHeat = simulationParameters.sourceHeat();
    for (std::size_t x = 0; x < simulationWidth; ++x) {
        const auto flicker = static_cast<std::uint8_t>(nextRandom() & 0x3Fu);
        const auto scaledFlicker = static_cast<std::uint16_t>(flicker) * sourceHeat / 255u;
        heatMap[rowOffset + x] = static_cast<std::uint8_t>(sourceHeat - scaledFlicker);
    }
}
