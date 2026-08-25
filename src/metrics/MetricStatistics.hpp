#pragma once

#include <cstddef>

struct MetricStatistics {
    double averageMilliseconds{0.0};
    double percentile95Milliseconds{0.0};
    double maximumMilliseconds{0.0};
    std::size_t sampleCount{0};
};
