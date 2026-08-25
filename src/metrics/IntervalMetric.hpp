#pragma once

#include "metrics/TimeSeriesMetric.hpp"

class IntervalMetric final {
public:
    using Clock = std::chrono::steady_clock;

    void mark(Clock::time_point now) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    TimeSeriesMetric intervals;
    Clock::time_point previousMark{};
    bool hasPreviousMark{false};
};
