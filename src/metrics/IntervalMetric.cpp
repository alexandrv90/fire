#include "metrics/IntervalMetric.hpp"

void IntervalMetric::mark(const MetricsClock::time_point now) noexcept {
    if (hasPreviousMark) {
        intervals.record(now - previousMark);
    }

    previousMark = now;
    hasPreviousMark = true;
}

void IntervalMetric::clear() noexcept {
    intervals.clear();
    previousMark = {};
    hasPreviousMark = false;
}

MetricStatistics IntervalMetric::statistics() const noexcept { return intervals.statistics(); }
