#pragma once

#include "metrics/MetricsClock.hpp"
#include "render/PixelBuffer.hpp"

#include <QImage>
#include <QWidget>

class FireView final : public QWidget {
    Q_OBJECT

public:
    explicit FireView(QWidget* parent = nullptr);

    void present(const PixelBuffer& pixels);

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

public slots:
    void setMetricsEnabled(bool enabled) noexcept { metricsEnabled = enabled; }

signals:
    void paintMeasured(MetricsClock::time_point startedAt, MetricsClock::duration duration);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawFrame();

    QImage frame;
    bool metricsEnabled{false};
};
