#pragma once

#include "metrics/MetricStatistics.hpp"

#include <array>
#include <chrono>
#include <cstddef>

class TimeSeriesMetric final {
public:
    using Clock = std::chrono::steady_clock;

    void record(Clock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    static constexpr std::size_t MAXIMUM_SAMPLE_COUNT = 512;

    std::array<Clock::duration, MAXIMUM_SAMPLE_COUNT> samples{};
    std::size_t nextSample{0};
    std::size_t sampleCount{0};
};
