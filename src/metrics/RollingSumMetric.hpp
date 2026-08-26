#pragma once

#include "metrics/MetricWindow.hpp"

#include <array>
#include <cstddef>

template <typename Value>
class RollingSumMetric final {
public:
    void record(const Value value) noexcept {
        if (retainedSampleCount == samples.size()) {
            accumulatedValue -= samples[nextSample];
        } else {
            ++retainedSampleCount;
        }

        samples[nextSample] = value;
        accumulatedValue += value;
        nextSample = (nextSample + 1) % samples.size();
    }

    void clear() noexcept {
        nextSample = 0;
        retainedSampleCount = 0;
        accumulatedValue = {};
    }

    [[nodiscard]] Value total() const noexcept { return accumulatedValue; }
    [[nodiscard]] std::size_t sampleCount() const noexcept { return retainedSampleCount; }

private:
    std::array<Value, METRIC_WINDOW_SAMPLE_COUNT> samples{};
    Value accumulatedValue{};
    std::size_t nextSample{0};
    std::size_t retainedSampleCount{0};
};
