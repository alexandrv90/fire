#include "app/FrameMetricsCollector.hpp"
#include "metrics/MetricWindow.hpp"
#include "tests_common.h"

#include <QObject>

#include <chrono>
#include <string_view>

namespace {
using namespace std::chrono_literals;

using fire_tests::check;
using fire_tests::checkNear;

void checkEmpty(const MetricStatistics& statistics, const std::string_view context) {
    check(statistics.sampleCount == 0, context);
    checkNear(statistics.averageMilliseconds, 0.0, context);
    checkNear(statistics.percentile95Milliseconds, 0.0, context);
    checkNear(statistics.maximumMilliseconds, 0.0, context);
}

void testCollectorRecordsEnabledObservations() {
    FrameMetricsCollector collector;
    int enabledTransitions = 0;
    bool reportedEnabledState = false;
    QObject::connect(&collector,
                     &FrameMetricsCollector::enabledChanged,
                     [&enabledTransitions, &reportedEnabledState](const bool enabled) {
                         ++enabledTransitions;
                         reportedEnabledState = enabled;
                     });

    collector.setEnabled(true);
    check(collector.isEnabled(), "collector enables at runtime");
    check(enabledTransitions == 1 && reportedEnabledState, "collector reports its enabled transition");

    const auto start = MetricsClock::time_point{};
    collector.observeWake(start);
    collector.observeWake(start + 10ms);

    const FrameReport report{2, 20ms, 1ms, 7, FrameStageTimings{3ms, 4ms}};
    collector.observeAdvance(report);
    collector.observeAdvance(FrameReport{0, 10ms, 2ms, 7, std::nullopt});
    collector.observePaint(start + 1ms, 5ms);
    collector.observePaint(start + 17ms, 7ms);

    const FrameMetricsSnapshot snapshot = collector.snapshot();
    checkNear(snapshot.simulateDuration.averageMilliseconds, 3.0, "collector records simulation duration");
    checkNear(snapshot.shadeDuration.averageMilliseconds, 4.0, "collector records shade duration");
    checkNear(snapshot.wakeInterval.averageMilliseconds, 10.0, "collector records wake intervals");
    checkNear(snapshot.paintDuration.averageMilliseconds, 6.0, "collector records paint duration");
    checkNear(snapshot.paintDuration.percentile95Milliseconds, 7.0, "collector reports paint duration p95");
    check(snapshot.paintDuration.sampleCount == 2, "collector counts paint duration samples");
    checkNear(snapshot.paintInterval.averageMilliseconds, 16.0, "collector records paint start intervals");
    check(snapshot.paintInterval.sampleCount == 1, "the first paint establishes the interval reference");
    check(snapshot.wakeActivity.discardedTime == 3ms, "collector accumulates discarded time");
    check(snapshot.wakeActivity.idleWakeCount == 1, "collector counts zero-tick wakes");
    check(snapshot.wakeActivity.sampleCount == 2, "collector counts activity samples");
    check(snapshot.latestFrameIndex == 7, "collector retains the latest produced frame index");
}

void testDisabledCollectorDoesNoCollectionWork() {
    FrameMetricsCollector collector;
    const auto start = MetricsClock::time_point{};

    collector.observeWake(start);
    collector.observeWake(start + 10ms);
    collector.observeAdvance(FrameReport{1, 17ms, 0ms, 1, FrameStageTimings{2ms, 3ms}});
    collector.observePaint(start, 4ms);

    const FrameMetricsSnapshot snapshot = collector.snapshot();
    check(!collector.isEnabled(), "collector starts disabled");
    checkEmpty(snapshot.simulateDuration, "disabled collector ignores simulation observations");
    checkEmpty(snapshot.shadeDuration, "disabled collector ignores shade observations");
    checkEmpty(snapshot.wakeInterval, "disabled collector ignores wake observations");
    checkEmpty(snapshot.paintDuration, "disabled collector ignores paint observations");
    checkEmpty(snapshot.paintInterval, "disabled collector ignores paint interval observations");
    check(snapshot.wakeActivity.sampleCount == 0, "disabled collector retains no wake activity");
    check(snapshot.latestFrameIndex == 0, "disabled collector retains no frame index");
}

void testEnableStartsFreshMeasurementSession() {
    FrameMetricsCollector collector;
    const auto start = MetricsClock::time_point{};

    collector.setEnabled(true);
    collector.observeWake(start);
    collector.observeWake(start + 5ms);
    collector.observeAdvance(FrameReport{1, 17ms, 0ms, 1, FrameStageTimings{2ms, 3ms}});
    collector.observePaint(start, 4ms);
    collector.setEnabled(false);

    collector.observeWake(start + 50ms);
    collector.observeAdvance(FrameReport{1, 17ms, 0ms, 2, FrameStageTimings{20ms, 30ms}});
    collector.observePaint(start + 50ms, 40ms);
    const FrameMetricsSnapshot disabledSnapshot = collector.snapshot();
    check(!collector.isEnabled(), "collector reports its disabled state");
    checkNear(
        disabledSnapshot.simulateDuration.averageMilliseconds, 2.0, "disabling freezes existing simulation statistics");
    check(disabledSnapshot.latestFrameIndex == 1, "disabled observations do not replace the latest frame index");

    collector.setEnabled(true);
    const FrameMetricsSnapshot reenabledSnapshot = collector.snapshot();
    check(collector.isEnabled(), "collector can be re-enabled");
    checkEmpty(reenabledSnapshot.simulateDuration, "re-enabling starts a fresh duration window");
    checkEmpty(reenabledSnapshot.wakeInterval, "re-enabling resets wake interval history");
    checkEmpty(reenabledSnapshot.paintInterval, "re-enabling resets paint interval history");
    check(reenabledSnapshot.wakeActivity.sampleCount == 0, "re-enabling clears wake activity");
    check(reenabledSnapshot.latestFrameIndex == 0, "re-enabling clears the previous frame index");
}

void testWakeActivityUsesRollingWindow() {
    FrameMetricsCollector collector;
    collector.setEnabled(true);
    collector.observeAdvance(FrameReport{0, 16ms, 5ms, 0, std::nullopt});

    for (std::size_t sample = 0; sample < METRIC_WINDOW_SAMPLE_COUNT; ++sample) {
        collector.observeAdvance(FrameReport{1, 16ms, 0ms, sample + 1, std::nullopt});
    }

    const WakeActivityStatistics activity = collector.snapshot().wakeActivity;
    check(activity.discardedTime == MetricsClock::duration{},
          "discarded time leaves the total when its sample is evicted");
    check(activity.idleWakeCount == 0, "idle wakes leave the total when their samples are evicted");
    check(activity.sampleCount == METRIC_WINDOW_SAMPLE_COUNT, "wake activity retains a fixed-size window");
}
} // namespace

int main() {
    testCollectorRecordsEnabledObservations();
    testDisabledCollectorDoesNoCollectionWork();
    testEnableStartsFreshMeasurementSession();
    testWakeActivityUsesRollingWindow();

    return fire_tests::reportResults("frame metrics collector");
}
