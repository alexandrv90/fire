#include "metrics/ScopedTimer.hpp"

ScopedTimer::ScopedTimer(TimeSeriesMetric& metric) noexcept
    : metric(metric), startedAt(TimeSeriesMetric::Clock::now()) {}

ScopedTimer::~ScopedTimer() noexcept { metric.record(TimeSeriesMetric::Clock::now() - startedAt); }
