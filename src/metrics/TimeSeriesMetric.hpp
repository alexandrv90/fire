#pragma once

#include "metrics/MetricStatistics.hpp"
#include "metrics/MetricWindow.hpp"
#include "metrics/MetricsClock.hpp"

#include <array>
#include <cstddef>

class TimeSeriesMetric final {
public:
    void record(MetricsClock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    std::array<MetricsClock::duration, METRIC_WINDOW_SAMPLE_COUNT> samples{};
    std::size_t nextSample{0};
    std::size_t sampleCount{0};
};
