#pragma once

#include "engine/PixelBuffer.hpp"
#include "metrics/MetricsClock.hpp"

#include <QImage>
#include <QWidget>

class FireView final : public QWidget {
    Q_OBJECT

public:
    explicit FireView(const PixelBuffer& pixels, QWidget* parent = nullptr);

    void present();

public slots:
    void setMetricsEnabled(bool enabled) noexcept { metricsEnabled = enabled; }

signals:
    void paintMeasured(MetricsClock::time_point startedAt, MetricsClock::duration duration);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QImage frame;
    bool metricsEnabled{false};
};
