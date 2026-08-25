#include "metrics/FrameProfiler.hpp"
#include "metrics/ScopedTimer.hpp"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace {
using namespace std::chrono_literals;

constexpr std::size_t MAXIMUM_SAMPLE_COUNT = 512;

int failureCount = 0;

void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    ++failureCount;
}

void checkNear(const double actual, const double expected, const std::string_view message) {
    constexpr double TOLERANCE = 1e-9;
    check(std::abs(actual - expected) <= TOLERANCE, message);
}

void checkEmpty(const MetricStatistics& statistics, const std::string_view context) {
    checkNear(statistics.averageMilliseconds, 0.0, context);
    checkNear(statistics.percentile95Milliseconds, 0.0, context);
    checkNear(statistics.maximumMilliseconds, 0.0, context);
    check(statistics.sampleCount == 0, context);
}

void testTimeSeriesStatistics() {
    TimeSeriesMetric metric;
    checkEmpty(metric.statistics(), "a new time series is empty");

    for (int milliseconds = 1; milliseconds <= 20; ++milliseconds) {
        metric.record(std::chrono::milliseconds{milliseconds});
    }

    const MetricStatistics statistics = metric.statistics();
    checkNear(statistics.averageMilliseconds, 10.5, "time series reports the average");
    checkNear(statistics.percentile95Milliseconds, 19.0, "time series reports nearest-rank p95");
    checkNear(statistics.maximumMilliseconds, 20.0, "time series reports the maximum");
    check(statistics.sampleCount == 20, "time series reports the sample count");

    metric.clear();
    checkEmpty(metric.statistics(), "clear empties a time series");
}

void testTimeSeriesRollingWindow() {
    TimeSeriesMetric metric;
    for (std::size_t sample = 0; sample < MAXIMUM_SAMPLE_COUNT; ++sample) {
        metric.record(1ms);
    }
    metric.record(2ms);

    const MetricStatistics statistics = metric.statistics();
    check(statistics.sampleCount == MAXIMUM_SAMPLE_COUNT, "time series retains a fixed-size window");
    checkNear(statistics.averageMilliseconds,
              513.0 / static_cast<double>(MAXIMUM_SAMPLE_COUNT),
              "time series evicts the oldest sample");
    checkNear(statistics.percentile95Milliseconds, 1.0, "rolling p95 uses retained samples only");
    checkNear(statistics.maximumMilliseconds, 2.0, "rolling maximum uses retained samples only");
}

void testIntervalMetric() {
    IntervalMetric metric;
    const IntervalMetric::Clock::time_point start{};

    metric.mark(start);
    checkEmpty(metric.statistics(), "the first interval mark establishes a reference");

    metric.mark(start + 10ms);
    metric.mark(start + 25ms);

    const MetricStatistics statistics = metric.statistics();
    checkNear(statistics.averageMilliseconds, 12.5, "interval metric reports the average gap");
    checkNear(statistics.percentile95Milliseconds, 15.0, "interval metric reports p95 gap");
    checkNear(statistics.maximumMilliseconds, 15.0, "interval metric reports the maximum gap");
    check(statistics.sampleCount == 2, "interval metric records each gap after the first mark");

    metric.clear();
    metric.mark(start + 100ms);
    checkEmpty(metric.statistics(), "clear resets the interval reference");
    metric.mark(start + 104ms);
    checkNear(metric.statistics().averageMilliseconds, 4.0, "interval metric records after a new reference");
}

void testScopedTimer() {
    static_assert(!std::is_copy_constructible_v<ScopedTimer>);
    static_assert(!std::is_copy_assignable_v<ScopedTimer>);

    TimeSeriesMetric metric;
    {
        const ScopedTimer timer{metric};
    }

    const MetricStatistics statistics = metric.statistics();
    check(statistics.sampleCount == 1, "scoped timer records one sample on destruction");
    check(statistics.averageMilliseconds >= 0.0, "scoped timer records a non-negative duration");
    checkNear(statistics.percentile95Milliseconds,
              statistics.averageMilliseconds,
              "a scoped timer's single sample is its p95");
    checkNear(statistics.maximumMilliseconds,
              statistics.averageMilliseconds,
              "a scoped timer's single sample is its maximum");
}

void testFrameProfilerClear() {
    FrameProfiler profiler;
    const IntervalMetric::Clock::time_point start{};

    profiler.simulate.record(1ms);
    profiler.shade.record(2ms);
    profiler.wakeInterval.mark(start);
    profiler.wakeInterval.mark(start + 3ms);
    profiler.presentInterval.mark(start);
    profiler.presentInterval.mark(start + 4ms);

    profiler.clear();

    checkEmpty(profiler.simulate.statistics(), "profiler clears simulate samples");
    checkEmpty(profiler.shade.statistics(), "profiler clears shade samples");
    checkEmpty(profiler.wakeInterval.statistics(), "profiler clears wake interval samples");
    checkEmpty(profiler.presentInterval.statistics(), "profiler clears present interval samples");

    profiler.wakeInterval.mark(start + 10ms);
    checkEmpty(profiler.wakeInterval.statistics(), "profiler clear resets interval references");
}
} // namespace

int main() {
    testTimeSeriesStatistics();
    testTimeSeriesRollingWindow();
    testIntervalMetric();
    testScopedTimer();
    testFrameProfilerClear();

    if (failureCount != 0) {
        std::cerr << failureCount << " metrics test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All metrics tests passed\n";
    return 0;
}
