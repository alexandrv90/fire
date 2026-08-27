#pragma once

#include "engine/FireRenderer.hpp"
#include "engine/FrameClock.hpp"
#include "engine/FrameReport.hpp"
#include "engine/PixelBuffer.hpp"
#include "sim/Dimensions.hpp"
#include "sim/FireParameters.hpp"
#include "sim/FireSimulation.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>

class FireEngine final {
public:
    FireEngine();
    explicit FireEngine(Dimensions dimensions);

    [[nodiscard]] static constexpr Dimensions defaultDimensions() noexcept { return DEFAULT_DIMENSIONS; }

    [[nodiscard]] FrameReport advance(std::chrono::steady_clock::duration elapsed) noexcept;
    void reset() noexcept;

    [[nodiscard]] Dimensions dimensions() const noexcept { return simulation.dimensions(); }
    [[nodiscard]] const PixelBuffer& frame() const noexcept { return renderedFrame; }

    [[nodiscard]] FirePalettePresetId palettePreset() const noexcept { return selectedPalettePreset; }
    void setPalettePreset(FirePalettePresetId preset) noexcept;

    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }
    void setParameters(const FireParameters& parameters) noexcept;

    void setStageTimingEnabled(bool enabled) noexcept { stageTimingEnabled = enabled; }

private:
    static constexpr Dimensions DEFAULT_DIMENSIONS{600, 400};
    static constexpr int MAXIMUM_TICKS_PER_WAKE = 3;
    static constexpr std::chrono::duration<double> TICK_DURATION{1.0 / 90.0}; // 90Hz

    void simulate(int ticks) noexcept;
    void shade() noexcept;

    FireSimulation simulation;
    PixelBuffer renderedFrame;
    FirePalettePresetId selectedPalettePreset{FirePalettePresetId::Classic};
    FireRenderer renderer;
    FrameClock clock{TICK_DURATION, MAXIMUM_TICKS_PER_WAKE};
    std::uint64_t frameIndex{0};
    bool stageTimingEnabled{false};
};
