#pragma once

#include "metrics/MetricsClock.hpp"
#include "metrics/TimeSeriesMetric.hpp"

class IntervalMetric final {
public:
    void mark(MetricsClock::time_point now) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    TimeSeriesMetric intervals;
    MetricsClock::time_point previousMark{};
    bool hasPreviousMark{false};
};
