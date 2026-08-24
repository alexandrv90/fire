#pragma once

#include <QImage>
#include <QWidget>

#include <array>
#include <cstdint>
#include <span>

class FireWidget final : public QWidget {
public:
    explicit FireWidget(int simulationWidth, int simulationHeight, QWidget* parent = nullptr);

    void present(std::span<const std::uint8_t> heat) noexcept;

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
