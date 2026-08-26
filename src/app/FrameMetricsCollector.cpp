#include "app/FrameMetricsCollector.hpp"

#include <utility>

FrameMetricsCollector::FrameMetricsCollector(QObject* const parent) : QObject(parent) {}

FrameMetricsSnapshot FrameMetricsCollector::snapshot() const noexcept {
    return {
        metricsEnabled,
        simulateDuration.statistics(),
        shadeDuration.statistics(),
        wakeInterval.statistics(),
        paintDuration.statistics(),
        paintInterval.statistics(),
        latestFrame,
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
    latestFrame.reset();
}

void FrameMetricsCollector::observeWake(const MetricsClock::time_point now) noexcept {
    if (metricsEnabled) {
        wakeInterval.mark(now);
    }
}

void FrameMetricsCollector::observeFrame(FrameReport report) noexcept {
    if (!metricsEnabled) {
        return;
    }

    if (report.stageTimings.has_value()) {
        simulateDuration.record(report.stageTimings->simulateDuration);
        shadeDuration.record(report.stageTimings->shadeDuration);
    }
    latestFrame = std::move(report);
}

void FrameMetricsCollector::observePaint(const MetricsClock::time_point startedAt,
                                         const MetricsClock::duration duration) noexcept {
    if (!metricsEnabled) {
        return;
    }

    paintDuration.record(duration);
    paintInterval.mark(startedAt);
}
