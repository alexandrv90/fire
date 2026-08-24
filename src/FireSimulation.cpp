#include "FireSimulation.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace {
constexpr std::uint32_t FALLBACK_SEED = 0xA341316Cu;
}

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

    // Propagate from each lower row into the row above it. Updating top-to-bottom
    // is intentional: every source row still contains the previous frame's heat.
    for (std::size_t y = 0; y + 1 < simulationHeight; ++y) {
        const std::size_t destinationOffset = y * simulationWidth;
        const std::size_t sourceOffset = destinationOffset + simulationWidth;

        for (std::size_t x = 0; x < simulationWidth; ++x) {
            const std::uint32_t random = nextRandom();
            std::ptrdiff_t drift = static_cast<std::ptrdiff_t>((random >> 8u) % 3u) - 1;

            const auto absoluteWindStrength =
                static_cast<std::uint32_t>(windStrength < 0 ? -windStrength : windStrength);
            if ((random & 0xFFu) < absoluteWindStrength * 24u) {
                drift += windStrength < 0 ? -1 : 1;
            }

            const auto sourceX = static_cast<std::ptrdiff_t>(x) - drift;
            // Treat space beyond the simulation as cold. Clamping here would copy
            // edge heat back into the area that wind has just vacated.
            std::uint8_t source = 0;
            if (sourceX >= 0 && sourceX < static_cast<std::ptrdiff_t>(simulationWidth)) {
                source = cells[sourceOffset + static_cast<std::size_t>(sourceX)];
            }
            const auto decay =
                static_cast<std::uint8_t>((random >> 24u) % (static_cast<std::uint32_t>(coolingRate) + 1u));

            cells[destinationOffset + x] = source > decay ? static_cast<std::uint8_t>(source - decay) : std::uint8_t{0};
        }
    }

    updateFuelRow();
}

void FireSimulation::reset() noexcept {
    std::fill(heatMap.begin(), heatMap.end(), std::uint8_t{0});
    randomState = initialSeed;
    updateFuelRow();
}

void FireSimulation::setCooling(const std::uint8_t cooling) noexcept {
    coolingRate = std::min(cooling, MAXIMUM_COOLING);
}

void FireSimulation::setWind(const int wind) noexcept { windStrength = std::clamp(wind, MINIMUM_WIND, MAXIMUM_WIND); }

std::uint32_t FireSimulation::nextRandom() noexcept {
    // Xorshift32 is compact, deterministic and adequate for visual noise.
    randomState ^= randomState << 13u;
    randomState ^= randomState >> 17u;
    randomState ^= randomState << 5u;
    return randomState;
}

void FireSimulation::updateFuelRow() noexcept {
    const std::size_t rowOffset = (simulationHeight - 1) * simulationWidth;
    for (std::size_t x = 0; x < simulationWidth; ++x) {
        const auto flicker = static_cast<std::uint8_t>(nextRandom() & 0x3Fu);
        const auto scaledFlicker = static_cast<std::uint16_t>(flicker) * sourceHeatLevel / 255u;
        heatMap[rowOffset + x] = static_cast<std::uint8_t>(sourceHeatLevel - scaledFlicker);
    }
}
