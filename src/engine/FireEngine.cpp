#include "engine/FireEngine.hpp"

#include "engine/FirePalette.hpp"

static_assert(FireEngine::defaultDimensions().width >= 2 && FireEngine::defaultDimensions().height >= 2);
static_assert(FireEngine::defaultDimensions().hasRepresentableArea());

FireEngine::FireEngine() : FireEngine(DEFAULT_DIMENSIONS) {}

FireEngine::FireEngine(const Dimensions dimensions)
    : simulation(dimensions), renderedFrame(dimensions), renderer(FirePalette::fromPreset(selectedPalettePreset)) {
    renderer.render(simulation.heat(), renderedFrame);
}

FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) noexcept {
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
    renderer.render(simulation.heat(), renderedFrame);
    clock.reset();
    frameIndex = 0;
}

void FireEngine::setPalettePreset(const FirePalettePresetId preset) noexcept {
    const FirePalettePresetId resolvedPreset = firePalettePreset(preset).id;
    if (resolvedPreset == selectedPalettePreset) {
        return;
    }

    const FirePalette palette = FirePalette::fromPreset(resolvedPreset);
    renderer.setPalette(palette);
    selectedPalettePreset = resolvedPreset;
    shade();
}

void FireEngine::setParameters(const FireParameters& parameters) noexcept { simulation.setParameters(parameters); }

void FireEngine::simulate(const int ticks) noexcept {
    for (int tick = 0; tick < ticks; ++tick) {
        simulation.tick();
    }
}

void FireEngine::shade() noexcept { renderer.render(simulation.heat(), renderedFrame); }
