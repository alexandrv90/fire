#pragma once

#include "sim/HeatFrame.hpp"

#include <QImage>
#include <QWidget>

#include <array>

class FireWidget final : public QWidget {
public:
    explicit FireWidget(QWidget* parent = nullptr);

    void present(const HeatFrame& heat) noexcept;

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    [[nodiscard]] static std::array<QRgb, 256> makePalette() noexcept;
    [[nodiscard]] QRect fittedFrameRect() const noexcept;

    QImage frame;
    std::array<QRgb, 256> palette;
};
