#include "app/FireWidget.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <array>
#include <cassert>
#include <limits>
#include <span>

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

FireWidget::FireWidget(QWidget* const parent) : QWidget(parent), palette(makePalette()) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::StrongFocus);
}

void FireWidget::present(const HeatFrame& heat) noexcept {
    constexpr auto MAXIMUM_IMAGE_DIMENSION = static_cast<std::size_t>(std::numeric_limits<int>::max());
    assert(heat.width() <= MAXIMUM_IMAGE_DIMENSION && heat.height() <= MAXIMUM_IMAGE_DIMENSION);
    if (heat.width() > MAXIMUM_IMAGE_DIMENSION || heat.height() > MAXIMUM_IMAGE_DIMENSION) {
        return;
    }

    const auto frameWidth = static_cast<int>(heat.width());
    const auto frameHeight = static_cast<int>(heat.height());
    if (frame.width() != frameWidth || frame.height() != frameHeight) {
        frame = QImage(frameWidth, frameHeight, QImage::Format_RGB32);
        assert(!frame.isNull());
        if (frame.isNull()) {
            return;
        }
    }

    for (std::size_t y = 0; y < heat.height(); ++y) {
        const std::span<const std::uint8_t> source = heat.row(y);
        auto* const destination = reinterpret_cast<QRgb*>(frame.scanLine(static_cast<int>(y)));
        for (std::size_t x = 0; x < source.size(); ++x) {
            destination[x] = palette[source[x]];
        }
    }

    update();
}

QSize FireWidget::minimumSizeHint() const { return {400, 300}; }

QSize FireWidget::sizeHint() const { return {800, 600}; }

void FireWidget::paintEvent(QPaintEvent* const event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(fittedFrameRect(), frame);
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
    if (availableWidth <= 0 || availableHeight <= 0 || frame.isNull()) {
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
