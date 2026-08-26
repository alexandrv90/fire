#include "metrics/TimeSeriesMetric.hpp"

#include <algorithm>

namespace {
double toMilliseconds(const MetricsClock::duration duration) noexcept {
    return std::chrono::duration<double, std::milli>{duration}.count();
}
} // namespace

void TimeSeriesMetric::record(const MetricsClock::duration duration) noexcept {
    samples[nextSample] = duration;
    nextSample = (nextSample + 1) % samples.size();
    sampleCount = std::min(sampleCount + 1, samples.size());
}

void TimeSeriesMetric::clear() noexcept {
    nextSample = 0;
    sampleCount = 0;
}

MetricStatistics TimeSeriesMetric::statistics() const noexcept {
    if (sampleCount == 0) {
        return {};
    }

    std::array<MetricsClock::duration, MAXIMUM_SAMPLE_COUNT> sortedSamples{};
    double totalMilliseconds = 0.0;
    for (std::size_t index = 0; index < sampleCount; ++index) {
        sortedSamples[index] = samples[index];
        totalMilliseconds += toMilliseconds(samples[index]);
    }

    const auto samplesEnd = sortedSamples.begin() + static_cast<std::ptrdiff_t>(sampleCount);
    std::sort(sortedSamples.begin(), samplesEnd);

    const std::size_t percentileIndex = (sampleCount * 95 + 99) / 100 - 1;
    return {
        totalMilliseconds / static_cast<double>(sampleCount),
        toMilliseconds(sortedSamples[percentileIndex]),
        toMilliseconds(sortedSamples[sampleCount - 1]),
        sampleCount,
    };
}
