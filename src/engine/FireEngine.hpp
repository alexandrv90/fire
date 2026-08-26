#pragma once

#include "engine/FireRenderer.hpp"
#include "engine/FrameClock.hpp"
#include "engine/FrameReport.hpp"
#include "sim/FireParameters.hpp"
#include "sim/FireSimulation.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>

class FireEngine final {
public:
    explicit FireEngine(std::size_t simulationWidth = SIMULATION_WIDTH,
                        std::size_t simulationHeight = SIMULATION_HEIGHT);

    [[nodiscard]] FrameReport advance(std::chrono::steady_clock::duration elapsed);
    void reset() noexcept;

    [[nodiscard]] const PixelBuffer& frame() const noexcept { return renderer.target(); }

    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }
    void setParameters(const FireParameters& parameters) noexcept;

    void setStageTimingEnabled(bool enabled) noexcept { stageTimingEnabled = enabled; }

private:
    static constexpr int SIMULATION_WIDTH = 800;
    static constexpr int SIMULATION_HEIGHT = 600;
    static constexpr int MAXIMUM_TICKS_PER_WAKE = 3;
    static constexpr std::chrono::duration<double> TICK_DURATION{1.0 / 60.0}; // 60Hz

    void simulate(int ticks) noexcept;
    void shade();

    FireSimulation simulation;
    FireRenderer renderer;
    FrameClock clock{TICK_DURATION, MAXIMUM_TICKS_PER_WAKE};
    std::uint64_t frameIndex{0};
    bool stageTimingEnabled{false};
};
