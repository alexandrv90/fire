#pragma once

#include "engine/PixelBuffer.hpp"
#include "metrics/MetricsClock.hpp"

#include <QImage>
#include <QWidget>

#include <cstddef>

class FireView final : public QWidget {
    Q_OBJECT

public:
    explicit FireView(const PixelBuffer& pixels, QWidget* parent = nullptr);

    void present();

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

public slots:
    void setMetricsEnabled(bool enabled) noexcept { metricsEnabled = enabled; }

signals:
    void paintMeasured(MetricsClock::time_point startedAt, MetricsClock::duration duration);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void rewrap(const PixelBuffer& pixels);

    QImage frame;
    const Rgba32* wrappedData{nullptr};
    std::size_t wrappedWidth{0};
    std::size_t wrappedHeight{0};
    bool metricsEnabled{false};
};
