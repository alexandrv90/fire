#pragma once

#include "sim/Dimensions.hpp"
#include "sim/FireParameters.hpp"
#include "sim/HeatFrame.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

// Heat diffuses upward through a scrolling value-noise cooling field. The noise
// stretches vertically to form coherent flame tongues, and horizontal wrapping
// prevents cold seams at the edges.
class FireSimulation final {
public:
    explicit FireSimulation(Dimensions dimensions, std::uint32_t randomSeed = 0xC001CAFEu);

    void tick() noexcept;
    void reset() noexcept;

    [[nodiscard]] Dimensions dimensions() const noexcept { return simulationDimensions; }

    // Converts the fixed-point field to palette indices on demand.
    [[nodiscard]] HeatFrame heat() const noexcept;

    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulationParameters; }
    void setParameters(const FireParameters& parameters) noexcept;

private:
    // Extra fractional bits prevent cumulative rounding loss as heat rises.
    using Cell = std::uint16_t;

    [[nodiscard]] std::uint32_t nextRandom() noexcept;

    [[nodiscard]] Cell* mutableRow(std::size_t y) noexcept;
    [[nodiscard]] const Cell* row(std::size_t y) const noexcept;

    void buildCoolingMap();
    void addNoiseOctave(std::size_t spacingX, std::size_t spacingY, double weight);
    void applyParameters() noexcept;
    void wrapEdges() noexcept;
    void refreshHeatMap() const noexcept;

    const Dimensions simulationDimensions;
    // The hidden surplus columns make the cooling lattice periodic.
    std::size_t fieldWidth;
    std::size_t fieldStride;

    std::vector<Cell> heatField;
    std::vector<std::uint16_t> coolingMap;
    mutable std::vector<std::uint8_t> heatMap;
    mutable bool heatMapStale{true};

    std::uint32_t randomState;
    std::uint32_t coolingAmplitude{0};
    std::size_t coolingScroll{0};
    FireParameters simulationParameters;
};
