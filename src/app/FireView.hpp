#pragma once

#include "metrics/IntervalMetric.hpp"
#include "render/PixelBuffer.hpp"

#include <QImage>
#include <QWidget>

class FireView final : public QWidget {
public:
    explicit FireView(IntervalMetric& presentInterval, QWidget* parent = nullptr);

    void present(const PixelBuffer& pixels);

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    IntervalMetric& presentInterval;
    QImage frame;
};
