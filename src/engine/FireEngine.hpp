#pragma once

#include "engine/FrameClock.hpp"
#include "engine/FrameReport.hpp"
#include "metrics/FrameProfiler.hpp"
#include "render/FireRenderer.hpp"
#include "sim/FireParameters.hpp"
#include "sim/FireSimulation.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>

class FireEngine final {
public:
    FireEngine(std::size_t simulationWidth, std::size_t simulationHeight);

    [[nodiscard]] FrameReport advance(std::chrono::steady_clock::duration elapsed);
    void reset() noexcept;

    [[nodiscard]] const PixelBuffer& frame() const noexcept { return renderer.target(); }

    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }
    void setParameters(const FireParameters& parameters) noexcept;

    [[nodiscard]] FrameProfiler& profiler() noexcept { return frameProfiler; }
    [[nodiscard]] const FrameProfiler& profiler() const noexcept { return frameProfiler; }

private:
    static constexpr int SIMULATION_TICKS_PER_SECOND = 60;
    static constexpr int MAXIMUM_TICKS_PER_WAKE = 3;

    FireSimulation simulation;
    FireRenderer renderer;
    FrameClock clock{SIMULATION_TICKS_PER_SECOND, MAXIMUM_TICKS_PER_WAKE};
    FrameProfiler frameProfiler;
    std::uint64_t frameIndex{0};
};
