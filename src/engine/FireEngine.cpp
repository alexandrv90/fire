#include "engine/FireEngine.hpp"

#include "render/FirePalette.hpp"

FireEngine::FireEngine(const std::size_t simulationWidth, const std::size_t simulationHeight)
    : simulation(simulationWidth, simulationHeight), renderer(FirePalette::classic()) {
    renderer.render(simulation.heat());
}

FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) {
    const TickPlan plan = clock.consume(elapsed);
    if (plan.ticks == 0) {
        return FrameReport{0, elapsed, plan.discardedTime, frameIndex, std::nullopt};
    }

    std::optional<FrameStageTimings> stageTimings;
    if (stageTimingEnabled) {
        const auto simulateStartedAt = std::chrono::steady_clock::now();
        simulate(plan.ticks);
        const auto simulateDuration = std::chrono::steady_clock::now() - simulateStartedAt;

        const auto shadeStartedAt = std::chrono::steady_clock::now();
        shade();
        const auto shadeDuration = std::chrono::steady_clock::now() - shadeStartedAt;
        stageTimings = FrameStageTimings{simulateDuration, shadeDuration};
    } else {
        simulate(plan.ticks);
        shade();
    }

    return FrameReport{plan.ticks, elapsed, plan.discardedTime, ++frameIndex, stageTimings};
}

void FireEngine::reset() noexcept {
    simulation.reset();
    renderer.render(simulation.heat());
    clock.reset();
    frameIndex = 0;
}

void FireEngine::setParameters(const FireParameters& parameters) noexcept { simulation.parameters() = parameters; }

void FireEngine::simulate(const int ticks) noexcept {
    for (int tick = 0; tick < ticks; ++tick) {
        simulation.tick();
    }
}

void FireEngine::shade() { renderer.render(simulation.heat()); }
