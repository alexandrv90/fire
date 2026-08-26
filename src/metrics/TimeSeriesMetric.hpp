#pragma once

#include "metrics/MetricStatistics.hpp"
#include "metrics/MetricsClock.hpp"

#include <array>
#include <cstddef>

class TimeSeriesMetric final {
public:
    void record(MetricsClock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    static constexpr std::size_t MAXIMUM_SAMPLE_COUNT = 512;

    std::array<MetricsClock::duration, MAXIMUM_SAMPLE_COUNT> samples{};
    std::size_t nextSample{0};
    std::size_t sampleCount{0};
};
