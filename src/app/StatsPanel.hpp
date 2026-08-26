#pragma once

#include <QWidget>

class FrameMetricsCollector;
class QLabel;
class QPaintEvent;

class StatsPanel final : public QWidget {
public:
    explicit StatsPanel(const FrameMetricsCollector& metricsCollector, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static constexpr int REFRESH_INTERVAL_MILLISECONDS = 250;

    void refresh();

    const FrameMetricsCollector& metricsCollector;
    QLabel* windowSummaryLabel{nullptr};
    QLabel* metricRowsLabel{nullptr};
    QLabel* latestFrameLabel{nullptr};
};
