fire_add_library(fire_metrics
    SOURCES
        IntervalMetric.cpp
        IntervalMetric.hpp
        MetricStatistics.hpp
        MetricWindow.hpp
        MetricsClock.hpp
        RollingSumMetric.hpp
        TimeSeriesMetric.cpp
        TimeSeriesMetric.hpp
)

fire_add_test(fire_metrics_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/metrics/MetricsTests.cpp"
    DEPENDENCIES
        fire_metrics
)
