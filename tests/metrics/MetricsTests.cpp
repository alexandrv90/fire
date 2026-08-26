#include "metrics/IntervalMetric.hpp"
#include "metrics/MetricWindow.hpp"
#include "metrics/RollingSumMetric.hpp"
#include "tests_common.h"

#include <chrono>
#include <cstddef>
#include <string_view>

namespace {
using namespace std::chrono_literals;

using fire_tests::check;
using fire_tests::checkNear;

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
    for (std::size_t sample = 0; sample < METRIC_WINDOW_SAMPLE_COUNT; ++sample) {
        metric.record(1ms);
    }
    metric.record(2ms);

    const MetricStatistics statistics = metric.statistics();
    check(statistics.sampleCount == METRIC_WINDOW_SAMPLE_COUNT, "time series retains a fixed-size window");
    checkNear(statistics.averageMilliseconds,
              513.0 / static_cast<double>(METRIC_WINDOW_SAMPLE_COUNT),
              "time series evicts the oldest sample");
    checkNear(statistics.percentile95Milliseconds, 1.0, "rolling p95 uses retained samples only");
    checkNear(statistics.maximumMilliseconds, 2.0, "rolling maximum uses retained samples only");
}

void testRollingSumMetric() {
    RollingSumMetric<MetricsClock::duration> metric;
    for (std::size_t sample = 0; sample < METRIC_WINDOW_SAMPLE_COUNT; ++sample) {
        metric.record(1ms);
    }
    metric.record(2ms);

    check(metric.sampleCount() == METRIC_WINDOW_SAMPLE_COUNT, "rolling sum retains a fixed-size window");
    check(metric.total() == 513ms, "rolling sum evicts the oldest value when adding a sample");

    for (std::size_t sample = 0; sample < METRIC_WINDOW_SAMPLE_COUNT; ++sample) {
        metric.record(1ms);
    }
    check(metric.total() == std::chrono::milliseconds{METRIC_WINDOW_SAMPLE_COUNT},
          "rolling sum removes a peak after it leaves the window");

    metric.clear();
    check(metric.sampleCount() == 0 && metric.total() == MetricsClock::duration{}, "clear empties a rolling sum");
}

void testIntervalMetric() {
    IntervalMetric metric;
    const MetricsClock::time_point start{};

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

} // namespace

int main() {
    testTimeSeriesStatistics();
    testTimeSeriesRollingWindow();
    testRollingSumMetric();
    testIntervalMetric();

    return fire_tests::reportResults("metrics");
}
