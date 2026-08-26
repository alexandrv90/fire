#pragma once

#include "engine/FrameReport.hpp"
#include "metrics/IntervalMetric.hpp"
#include "metrics/MetricStatistics.hpp"
#include "metrics/MetricsClock.hpp"
#include "metrics/TimeSeriesMetric.hpp"

#include <QObject>

#include <optional>

struct FrameMetricsSnapshot {
    bool enabled{false};
    MetricStatistics simulateDuration;
    MetricStatistics shadeDuration;
    MetricStatistics wakeInterval;
    MetricStatistics paintDuration;
    MetricStatistics paintInterval;
    std::optional<FrameReport> latestFrame;
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
    void observeFrame(FrameReport report) noexcept;
    void observePaint(MetricsClock::time_point startedAt, MetricsClock::duration duration) noexcept;

signals:
    void enabledChanged(bool enabled);

private:
    TimeSeriesMetric simulateDuration;
    TimeSeriesMetric shadeDuration;
    IntervalMetric wakeInterval;
    TimeSeriesMetric paintDuration;
    IntervalMetric paintInterval;
    std::optional<FrameReport> latestFrame;
    bool metricsEnabled{false};
};
