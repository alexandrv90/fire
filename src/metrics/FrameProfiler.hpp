#pragma once

#include "metrics/IntervalMetric.hpp"
#include "metrics/TimeSeriesMetric.hpp"

struct FrameProfiler final {
    TimeSeriesMetric simulate;
    TimeSeriesMetric shade;
    IntervalMetric wakeInterval;
    IntervalMetric presentInterval;

    void clear() noexcept;
};
