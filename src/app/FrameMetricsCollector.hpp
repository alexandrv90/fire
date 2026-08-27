#pragma once

#include "engine/FrameReport.hpp"
#include "metrics/IntervalMetric.hpp"
#include "metrics/MetricStatistics.hpp"
#include "metrics/MetricsClock.hpp"
#include "metrics/RollingSumMetric.hpp"
#include "metrics/TimeSeriesMetric.hpp"

#include <QObject>

#include <cstddef>
#include <cstdint>

struct WakeActivityStatistics {
    MetricsClock::duration discardedTime{};
    std::size_t idleWakeCount{0};
    std::size_t sampleCount{0};
};

struct FrameMetricsSnapshot {
    MetricStatistics simulateDuration;
    MetricStatistics shadeDuration;
    MetricStatistics wakeInterval;
    MetricStatistics paintDuration;
    MetricStatistics paintInterval;
    WakeActivityStatistics wakeActivity;
    std::uint64_t latestFrameIndex{0};
};

class FrameMetricsCollector final : public QObject {
    Q_OBJECT

public:
    explicit FrameMetricsCollector(QObject* parent = nullptr);

    [[nodiscard]] bool isEnabled() const noexcept { return metricsEnabled; }
    [[nodiscard]] FrameMetricsSnapshot snapshot() const noexcept;

public slots:
    void setEnabled(bool enabled);
    void clear() noexcept;

    void observeWake(MetricsClock::time_point now) noexcept;
    void observeAdvance(FrameReport report) noexcept;
    void observePaint(MetricsClock::time_point startedAt, MetricsClock::duration duration) noexcept;

signals:
    void enabledChanged(bool enabled);

private:
    TimeSeriesMetric simulateDuration;
    TimeSeriesMetric shadeDuration;
    TimeSeriesMetric paintDuration;
    IntervalMetric paintInterval;
    IntervalMetric wakeInterval;
    RollingSumMetric<MetricsClock::duration> discardedTime;
    RollingSumMetric<std::size_t> idleWakeCount;
    std::uint64_t latestFrameIndex{0};
    bool metricsEnabled{false};
};
