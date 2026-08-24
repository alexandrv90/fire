#pragma once

#include <QElapsedTimer>
#include <QImage>
#include <QString>
#include <QWidget>

#include <array>
#include <cstdint>
#include <span>

class FireWidget final : public QWidget {
public:
    explicit FireWidget(int simulationWidth, int simulationHeight, QWidget* parent = nullptr);

    void present(std::span<const std::uint8_t> heat) noexcept;
    void setPaused(bool paused);

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    [[nodiscard]] static std::array<QRgb, 256> makePalette() noexcept;
    [[nodiscard]] QRect fittedFrameRect() const noexcept;
    void updateFrameRateText();

    QImage frame;
    std::array<QRgb, 256> palette;
    QElapsedTimer frameRateTimer;
    QString frameRateText{QStringLiteral("FPS: --")};
    int presentedFrames{0};
    bool isPaused{false};
};
