#pragma once

#include "sim/FireParameters.hpp"
#include "sim/HeatFrame.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class FireSimulation final {
public:
    explicit FireSimulation(std::size_t width, std::size_t height, std::uint32_t randomSeed = 0xC001CAFEu);

    void tick() noexcept;
    void reset() noexcept;

    [[nodiscard]] std::size_t width() const noexcept { return simulationWidth; }
    [[nodiscard]] std::size_t height() const noexcept { return simulationHeight; }
    [[nodiscard]] HeatFrame heat() const noexcept { return {heatMap, simulationWidth, simulationHeight}; }

    // Parameters clamp themselves, so exposing them mutably cannot invalidate the
    // simulation: tick() re-reads them each frame and holds no derived state.
    [[nodiscard]] FireParameters& parameters() noexcept { return simulationParameters; }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulationParameters; }

private:
    [[nodiscard]] std::uint32_t nextRandom() noexcept;
    void updateFuelRow() noexcept;

    std::size_t simulationWidth;
    std::size_t simulationHeight;
    std::vector<std::uint8_t> heatMap;
    std::uint32_t initialSeed;
    std::uint32_t randomState;
    FireParameters simulationParameters;
};
