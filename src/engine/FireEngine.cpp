#include "engine/FireEngine.hpp"

#include "metrics/ScopedTimer.hpp"
#include "render/FirePalette.hpp"

FireEngine::FireEngine(const std::size_t simulationWidth, const std::size_t simulationHeight)
    : simulation(simulationWidth, simulationHeight), renderer(FirePalette::classic()) {
    renderer.render(simulation.heat());
}

FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) {
    const TickPlan plan = clock.consume(elapsed);
    if (plan.ticks == 0) {
        return FrameReport{0, elapsed, plan.discardedTime, frameIndex};
    }

    {
        const ScopedTimer timer{frameProfiler.simulate};
        for (int tick = 0; tick < plan.ticks; ++tick) {
            simulation.tick();
        }
    }
    {
        const ScopedTimer timer{frameProfiler.shade};
        renderer.render(simulation.heat());
    }

    return FrameReport{plan.ticks, elapsed, plan.discardedTime, ++frameIndex};
}

void FireEngine::reset() noexcept {
    simulation.reset();
    renderer.render(simulation.heat());
    clock.reset();
    frameProfiler.clear();
    frameIndex = 0;
}

void FireEngine::setParameters(const FireParameters& parameters) noexcept { simulation.parameters() = parameters; }
