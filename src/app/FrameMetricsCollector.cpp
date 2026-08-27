#include "app/FrameMetricsCollector.hpp"

FrameMetricsCollector::FrameMetricsCollector(QObject* const parent) : QObject(parent) {}

FrameMetricsSnapshot FrameMetricsCollector::snapshot() const noexcept {
    return {
        simulateDuration.statistics(),
        shadeDuration.statistics(),
        wakeInterval.statistics(),
        paintDuration.statistics(),
        paintInterval.statistics(),
        WakeActivityStatistics{
            discardedTime.total(),
            idleWakeCount.total(),
            discardedTime.sampleCount(),
        },
        latestFrameIndex,
    };
}

void FrameMetricsCollector::setEnabled(const bool enabled) {
    if (metricsEnabled == enabled) {
        return;
    }

    if (enabled) {
        clear();
    }
    metricsEnabled = enabled;
    emit enabledChanged(metricsEnabled);
}

void FrameMetricsCollector::clear() noexcept {
    simulateDuration.clear();
    shadeDuration.clear();
    wakeInterval.clear();
    paintDuration.clear();
    paintInterval.clear();
    discardedTime.clear();
    idleWakeCount.clear();
    latestFrameIndex = 0;
}

void FrameMetricsCollector::observeWake(const MetricsClock::time_point now) noexcept {
    if (!metricsEnabled) {
        return;
    }

    wakeInterval.mark(now);
}

void FrameMetricsCollector::observeAdvance(const FrameReport report) noexcept {
    if (!metricsEnabled) {
        return;
    }

    if (report.stageTimings.has_value()) {
        simulateDuration.record(report.stageTimings->simulateDuration);
        shadeDuration.record(report.stageTimings->shadeDuration);
    }
    discardedTime.record(report.discardedTime);
    idleWakeCount.record(report.ticksExecuted == 0 ? 1U : 0U);
    latestFrameIndex = report.frameIndex;
}

void FrameMetricsCollector::observePaint(const MetricsClock::time_point startedAt,
                                         const MetricsClock::duration duration) noexcept {
    if (!metricsEnabled) {
        return;
    }

    paintDuration.record(duration);
    paintInterval.mark(startedAt);
}
