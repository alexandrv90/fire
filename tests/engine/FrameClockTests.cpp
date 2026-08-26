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

constexpr TickDuration PRODUCTION_TICK_DURATION{1.0 / 60.0};

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
    FrameClock clock{PRODUCTION_TICK_DURATION, 3};

    checkPlan(clock.consume(16ms), 0, 0ns, "a sixteen millisecond wake is shorter than one 60 Hz step");
    checkPlan(clock.consume(1ms), 1, 0ns, "the next wake drains the accumulated 60 Hz step");
}

void testResetClearsAccumulatedTime() {
    FrameClock clock{250ms, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step exists before reset");
    clock.reset();
    checkPlan(clock.consume(125ms), 0, 0ns, "reset clears partial elapsed time");
    checkPlan(clock.consume(125ms), 1, 0ns, "the clock accumulates normally after reset");
}

[[nodiscard]] std::vector<Rgba32> copyPixels(const PixelBuffer& frame) {
    return {frame.data(), frame.data() + frame.width() * frame.height()};
}

void testEngineProducesOnlyTickedFrames() {
    FireEngine engine{8, 6};
    const Rgba32* const initialStorage = engine.frame().data();
    const std::vector<Rgba32> initialPixels = copyPixels(engine.frame());
    check(engine.frame().width() == 8 && engine.frame().height() == 6,
          "the engine initializes a rendered frame with simulation geometry");

    const FrameReport idleReport = engine.advance(16ms);
    check(idleReport.ticksExecuted == 0, "an engine wake shorter than one step executes no ticks");
    check(idleReport.elapsed == 16ms, "an engine report retains its elapsed wall time");
    check(idleReport.discardedTime == 0ns, "an ordinary engine wake discards no time");
    check(idleReport.frameIndex == 0, "a zero-tick wake does not advance the frame index");
    check(!idleReport.stageTimings.has_value(), "a zero-tick wake reports no stage timings");
    check(engine.frame().data() == initialStorage && copyPixels(engine.frame()) == initialPixels,
          "a zero-tick wake leaves the rendered frame untouched");

    engine.setStageTimingEnabled(true);
    const FrameReport frameReport = engine.advance(1ms);
    check(frameReport.ticksExecuted == 1, "accumulated engine time executes a simulation tick");
    check(frameReport.frameIndex == 1, "a produced frame advances the frame index");
    check(frameReport.stageTimings.has_value(), "an enabled engine reports produced-frame stage timings");
    check(frameReport.stageTimings->simulateDuration >= 0ns, "engine reports a non-negative simulation duration");
    check(frameReport.stageTimings->shadeDuration >= 0ns, "engine reports a non-negative shade duration");
    check(engine.frame().data() == initialStorage, "engine rendering reuses its pixel storage");
    check(copyPixels(engine.frame()) != initialPixels, "a ticked engine shades the updated simulation");

    engine.setStageTimingEnabled(false);
    const FrameReport unmeasuredReport = engine.advance(17ms);
    check(unmeasuredReport.ticksExecuted == 1 && !unmeasuredReport.stageTimings.has_value(),
          "a disabled engine produces frames without measuring stage timings");
}

void testEngineReportsCatchUpAndParameters() {
    FireEngine engine{8, 6};
    FireParameters parameters = engine.parameters();
    parameters.setSourceHeat(96);
    parameters.setCooling(7);
    engine.setParameters(parameters);

    check(engine.parameters().sourceHeat() == 96 && engine.parameters().cooling() == 7,
          "the engine accepts a complete parameter value");

    const FrameReport report = engine.advance(100ms);
    check(report.ticksExecuted == 3, "the engine applies the maximum ticks per wake");
    check(report.discardedTime > 0ns, "the engine reports time discarded by its catch-up clamp");
    check(report.frameIndex == 1, "multiple ticks in one wake produce one frame");
}

void testEngineResetRestoresInitialState() {
    FireEngine engine{8, 6};
    const std::vector<Rgba32> initialPixels = copyPixels(engine.frame());

    engine.setStageTimingEnabled(true);
    static_cast<void>(engine.advance(16ms));
    static_cast<void>(engine.advance(1ms));
    engine.reset();

    check(copyPixels(engine.frame()) == initialPixels, "engine reset restores the initial rendered frame");
    const FrameReport idleReport = engine.advance(1ms);
    check(idleReport.ticksExecuted == 0 && idleReport.frameIndex == 0,
          "engine reset clears accumulated time and restarts frame indexing");

    const FrameReport frameReport = engine.advance(16ms);
    check(frameReport.ticksExecuted == 1 && frameReport.frameIndex == 1, "engine produces frames normally after reset");
    check(frameReport.stageTimings.has_value(), "engine reset preserves the stage timing policy");
}
} // namespace

int main() {
    testElapsedTimeAccumulates();
    testCatchUpClampReportsDiscardedTime();
    testProductionTickDurationBoundary();
    testResetClearsAccumulatedTime();
    testEngineProducesOnlyTickedFrames();
    testEngineReportsCatchUpAndParameters();
    testEngineResetRestoresInitialState();

    return fire_tests::reportResults("engine");
}
