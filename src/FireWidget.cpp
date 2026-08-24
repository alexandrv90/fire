#include "FireWidget.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <algorithm>
#include <array>
#include <cassert>

namespace {
struct PaletteStop {
    int index;
    int red;
    int green;
    int blue;
};

constexpr std::array PALETTE_STOPS{
    PaletteStop{0, 0, 0, 0},
    PaletteStop{32, 45, 0, 0},
    PaletteStop{80, 150, 0, 0},
    PaletteStop{136, 255, 45, 0},
    PaletteStop{192, 255, 160, 0},
    PaletteStop{232, 255, 245, 80},
    PaletteStop{255, 255, 255, 255},
};
} // namespace

FireWidget::FireWidget(const int simulationWidth, const int simulationHeight, QWidget* const parent)
    : QWidget(parent), frame(simulationWidth, simulationHeight, QImage::Format_RGB32), palette(makePalette()) {
    assert(!frame.isNull());
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::StrongFocus);
    frame.fill(Qt::black);
    frameRateTimer.start();
}

void FireWidget::present(const std::span<const std::uint8_t> heat) noexcept {
    const auto expectedSize = static_cast<std::size_t>(frame.width()) * static_cast<std::size_t>(frame.height());
    assert(heat.size() == expectedSize);
    if (heat.size() != expectedSize) {
        return;
    }

    std::size_t sourceOffset = 0;
    for (int y = 0; y < frame.height(); ++y) {
        auto* const destination = reinterpret_cast<QRgb*>(frame.scanLine(y));
        for (int x = 0; x < frame.width(); ++x) {
            destination[x] = palette[heat[sourceOffset++]];
        }
    }

    update();
}

void FireWidget::setPaused(const bool paused) {
    if (isPaused == paused) {
        return;
    }
    isPaused = paused;
    presentedFrames = 0;
    frameRateTimer.restart();
    frameRateText = isPaused ? QStringLiteral("FPS: paused") : QStringLiteral("FPS: --");
    update();
}

QSize FireWidget::minimumSizeHint() const { return {400, 300}; }

QSize FireWidget::sizeHint() const { return {800, 600}; }

void FireWidget::paintEvent(QPaintEvent* const event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, /* false */ true);
    painter.drawImage(fittedFrameRect(), frame);

    if (!isPaused) {
        ++presentedFrames;
        updateFrameRateText();
    }

    const QRect overlayRect{12, 12, 124, 34};
    painter.fillRect(overlayRect, QColor{0, 0, 0, 168});
    painter.setPen(QColor{255, 244, 190});
    painter.drawText(overlayRect.adjusted(10, 0, -6, 0), Qt::AlignLeft | Qt::AlignVCenter, frameRateText);
}

std::array<QRgb, 256> FireWidget::makePalette() noexcept {
    std::array<QRgb, 256> palette{};

    for (std::size_t segment = 0; segment + 1 < PALETTE_STOPS.size(); ++segment) {
        const PaletteStop& first = PALETTE_STOPS[segment];
        const PaletteStop& second = PALETTE_STOPS[segment + 1];
        const int distance = second.index - first.index;

        for (int index = first.index; index <= second.index; ++index) {
            const int offset = index - first.index;
            const int red = first.red + (second.red - first.red) * offset / distance;
            const int green = first.green + (second.green - first.green) * offset / distance;
            const int blue = first.blue + (second.blue - first.blue) * offset / distance;
            palette[static_cast<std::size_t>(index)] = qRgb(red, green, blue);
        }
    }

    return palette;
}

QRect FireWidget::fittedFrameRect() const noexcept {
    const int availableWidth = width();
    const int availableHeight = height();
    if (availableWidth <= 0 || availableHeight <= 0) {
        return {};
    }

    const qint64 widthLimitedHeight = static_cast<qint64>(availableWidth) * frame.height();
    const qint64 heightLimitedWidth = static_cast<qint64>(availableHeight) * frame.width();

    int renderedWidth = availableWidth;
    int renderedHeight = availableHeight;
    if (widthLimitedHeight <= heightLimitedWidth) {
        renderedHeight = static_cast<int>(widthLimitedHeight / frame.width());
    } else {
        renderedWidth = static_cast<int>(heightLimitedWidth / frame.height());
    }

    return {
        (availableWidth - renderedWidth) / 2, (availableHeight - renderedHeight) / 2, renderedWidth, renderedHeight};
}

void FireWidget::updateFrameRateText() {
    const qint64 elapsedMilliseconds = frameRateTimer.elapsed();
    if (elapsedMilliseconds < 500) {
        return;
    }

    const double framesPerSecond =
        static_cast<double>(presentedFrames) * 1000.0 / static_cast<double>(elapsedMilliseconds);
    frameRateText = QStringLiteral("FPS: %1").arg(framesPerSecond, 0, 'f', 1);
    presentedFrames = 0;
    frameRateTimer.restart();
}
