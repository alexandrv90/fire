#pragma once

#include "metrics/TimeSeriesMetric.hpp"

class ScopedTimer final {
public:
    explicit ScopedTimer(TimeSeriesMetric& metric) noexcept;
    ~ScopedTimer() noexcept;

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    TimeSeriesMetric& metric;
    TimeSeriesMetric::Clock::time_point startedAt;
};
