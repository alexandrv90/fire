#include "engine/FireEngine.hpp"
#include "engine/FrameClock.hpp"
#include "tests_common.h"

#include <chrono>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {
using namespace std::chrono_literals;

using fire_tests::check;

using TickDuration = std::chrono::duration<double>;
using ClockDuration = std::chrono::steady_clock::duration;

constexpr TickDuration PRODUCTION_TICK_DURATION{1.0 / 90.0};
constexpr int PRODUCTION_MAXIMUM_TICKS_PER_WAKE = 3;
constexpr ClockDuration PRODUCTION_PARTIAL_TICK_ELAPSED = std::chrono::floor<ClockDuration>(PRODUCTION_TICK_DURATION);
constexpr ClockDuration PRODUCTION_TICK_COMPLETION_ELAPSED =
    std::chrono::ceil<ClockDuration>(PRODUCTION_TICK_DURATION) - PRODUCTION_PARTIAL_TICK_ELAPSED;
constexpr ClockDuration PRODUCTION_TICK_ELAPSED = std::chrono::ceil<ClockDuration>(PRODUCTION_TICK_DURATION);
constexpr ClockDuration PRODUCTION_CATCH_UP_ELAPSED =
    std::chrono::ceil<ClockDuration>(PRODUCTION_TICK_DURATION * (PRODUCTION_MAXIMUM_TICKS_PER_WAKE + 1));

void checkPlan(const TickPlan& plan,
               const int expectedTicks,
               const std::chrono::steady_clock::duration expectedDiscardedTime,
               const std::string_view message) {
    check(plan.ticks == expectedTicks && plan.discardedTime == expectedDiscardedTime, message);
}

void testElapsedTimeAccumulates() {
    FrameClock clock{250ms, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step produces no tick");
    checkPlan(clock.consume(125ms), 1, 0ns, "partial elapsed times accumulate into a tick");
    checkPlan(clock.consume(500ms), 2, 0ns, "one wake can produce multiple ticks");
}

void testCatchUpClampReportsDiscardedTime() {
    FrameClock clock{250ms, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step is retained before a delayed wake");
    checkPlan(clock.consume(1s), 3, 375ms, "the catch-up clamp reports time it discards");
    checkPlan(clock.consume(0ns), 0, 0ns, "the clamped catch-up plan leaves no hidden backlog");
}

void testProductionTickDurationBoundary() {
    FrameClock clock{PRODUCTION_TICK_DURATION, PRODUCTION_MAXIMUM_TICKS_PER_WAKE};

    checkPlan(clock.consume(PRODUCTION_PARTIAL_TICK_ELAPSED),
              0,
              0ns,
              "an elapsed duration below the production tick duration produces no tick");
    checkPlan(clock.consume(PRODUCTION_TICK_COMPLETION_ELAPSED),
              1,
              0ns,
              "the next wake completes the accumulated production tick");
}

void testResetClearsAccumulatedTime() {
    FrameClock clock{250ms, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step exists before reset");
    clock.reset();
    checkPlan(clock.consume(125ms), 0, 0ns, "reset clears partial elapsed time");
    checkPlan(clock.consume(125ms), 1, 0ns, "the clock accumulates normally after reset");
}

[[nodiscard]] std::vector<Rgba32> copyPixels(const PixelBuffer& frame) {
    return {frame.data(), frame.data() + frame.dimensions().area()};
}

void testEngineProducesOnlyTickedFrames() {
    FireEngine engine{{8, 6}};
    const Rgba32* const initialStorage = engine.frame().data();
    const std::vector<Rgba32> initialPixels = copyPixels(engine.frame());
    check(engine.dimensions() == Dimensions{8, 6}, "the engine retains its configured simulation geometry");
    check(engine.frame().dimensions() == engine.dimensions(),
          "the engine initializes a rendered frame with simulation geometry");

    const FrameReport idleReport = engine.advance(PRODUCTION_PARTIAL_TICK_ELAPSED);
    check(idleReport.ticksExecuted == 0, "an engine wake shorter than one step executes no ticks");
    check(idleReport.elapsed == PRODUCTION_PARTIAL_TICK_ELAPSED, "an engine report retains its elapsed wall time");
    check(idleReport.discardedTime == 0ns, "an ordinary engine wake discards no time");
    check(idleReport.frameIndex == 0, "a zero-tick wake does not advance the frame index");
    check(!idleReport.stageTimings.has_value(), "a zero-tick wake reports no stage timings");
    check(engine.frame().data() == initialStorage && copyPixels(engine.frame()) == initialPixels,
          "a zero-tick wake leaves the rendered frame untouched");

    engine.setStageTimingEnabled(true);
    const FrameReport frameReport = engine.advance(PRODUCTION_TICK_COMPLETION_ELAPSED);
    check(frameReport.ticksExecuted == 1, "accumulated engine time executes a simulation tick");
    check(frameReport.frameIndex == 1, "a produced frame advances the frame index");
    check(frameReport.stageTimings.has_value(), "an enabled engine reports produced-frame stage timings");
    check(frameReport.stageTimings->simulateDuration >= 0ns, "engine reports a non-negative simulation duration");
    check(frameReport.stageTimings->shadeDuration >= 0ns, "engine reports a non-negative shade duration");
    check(engine.frame().data() == initialStorage, "engine rendering reuses its pixel storage");
    check(copyPixels(engine.frame()) != initialPixels, "a ticked engine shades the updated simulation");

    engine.setStageTimingEnabled(false);
    const FrameReport unmeasuredReport = engine.advance(PRODUCTION_TICK_ELAPSED);
    check(unmeasuredReport.ticksExecuted == 1 && !unmeasuredReport.stageTimings.has_value(),
          "a disabled engine produces frames without measuring stage timings");
}

void testEngineUsesCompileTimeDefaultDimensions() {
    FireEngine engine;
    check(engine.dimensions() == FireEngine::defaultDimensions(),
          "the default engine uses its compile-time dimensions");
    check(engine.frame().dimensions() == engine.dimensions(), "the default rendered frame uses the engine dimensions");
}

void testEngineReportsCatchUpAndParameters() {
    FireEngine engine{{8, 6}};
    FireParameters parameters = engine.parameters();
    parameters.setSourceHeat(96);
    parameters.setCooling(7);
    engine.setParameters(parameters);

    check(engine.parameters().sourceHeat() == 96 && engine.parameters().cooling() == 7,
          "the engine accepts a complete parameter value");

    const FrameReport report = engine.advance(PRODUCTION_CATCH_UP_ELAPSED);
    check(report.ticksExecuted == PRODUCTION_MAXIMUM_TICKS_PER_WAKE, "the engine applies the maximum ticks per wake");
    check(report.discardedTime > 0ns, "the engine reports time discarded by its catch-up clamp");
    check(report.frameIndex == 1, "multiple ticks in one wake produce one frame");
}

void testEngineResetRestoresInitialState() {
    FireEngine engine{{8, 6}};
    const std::vector<Rgba32> initialPixels = copyPixels(engine.frame());

    engine.setStageTimingEnabled(true);
    static_cast<void>(engine.advance(PRODUCTION_PARTIAL_TICK_ELAPSED));
    static_cast<void>(engine.advance(PRODUCTION_TICK_COMPLETION_ELAPSED));
    engine.reset();

    check(copyPixels(engine.frame()) == initialPixels, "engine reset restores the initial rendered frame");
    const FrameReport idleReport = engine.advance(PRODUCTION_TICK_COMPLETION_ELAPSED);
    check(idleReport.ticksExecuted == 0 && idleReport.frameIndex == 0,
          "engine reset clears accumulated time and restarts frame indexing");

    const FrameReport frameReport = engine.advance(PRODUCTION_PARTIAL_TICK_ELAPSED);
    check(frameReport.ticksExecuted == 1 && frameReport.frameIndex == 1, "engine produces frames normally after reset");
    check(frameReport.stageTimings.has_value(), "engine reset preserves the stage timing policy");
}

void testEngineSwitchesPaletteWithoutResettingSimulation() {
    FireEngine engine{{8, 6}};
    static_cast<void>(engine.advance(PRODUCTION_TICK_ELAPSED));
    const Rgba32* const frameStorage = engine.frame().data();
    const std::vector<Rgba32> classicPixels = copyPixels(engine.frame());

    engine.setPalettePreset(FirePalettePresetId::Ghostlight);
    check(engine.palettePreset() == FirePalettePresetId::Ghostlight, "the engine reports the selected palette preset");
    check(engine.frame().data() == frameStorage, "switching palettes reuses the rendered frame storage");
    check(copyPixels(engine.frame()) != classicPixels,
          "switching palettes immediately re-shades the current simulation frame");

    engine.setPalettePreset(FirePalettePresetId::Classic);
    check(copyPixels(engine.frame()) == classicPixels,
          "switching back to classic re-shades the unchanged simulation heat");

    engine.setPalettePreset(static_cast<FirePalettePresetId>(255));
    check(engine.palettePreset() == FirePalettePresetId::Classic,
          "an unknown palette preset leaves the engine on classic");

    engine.setPalettePreset(FirePalettePresetId::ArcaneBloom);
    engine.reset();
    check(engine.palettePreset() == FirePalettePresetId::ArcaneBloom, "reset preserves the selected palette preset");
}
} // namespace

int main() {
    testElapsedTimeAccumulates();
    testCatchUpClampReportsDiscardedTime();
    testProductionTickDurationBoundary();
    testResetClearsAccumulatedTime();
    testEngineUsesCompileTimeDefaultDimensions();
    testEngineProducesOnlyTickedFrames();
    testEngineReportsCatchUpAndParameters();
    testEngineResetRestoresInitialState();
    testEngineSwitchesPaletteWithoutResettingSimulation();

    return fire_tests::reportResults("engine");
}
