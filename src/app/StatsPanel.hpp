#pragma once

#include <QWidget>

class FrameMetricsCollector;
class QLabel;
class QPaintEvent;
class QTimer;

class StatsPanel final : public QWidget {
    Q_OBJECT
public:
    explicit StatsPanel(const FrameMetricsCollector& metricsCollector, QWidget* parent = nullptr);
    void setSuspended(bool suspended);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static constexpr int REFRESH_INTERVAL_MILLISECONDS = 250;

    void refresh();
    void updateRefreshTimer();

    const FrameMetricsCollector& metricsCollector;
    QLabel* columnHeaderLabel{nullptr};
    QLabel* metricRowsLabel{nullptr};
    QLabel* frameSummaryLabel{nullptr};
    QTimer* refreshTimer{nullptr};
    bool suspensionActive{false};
};
