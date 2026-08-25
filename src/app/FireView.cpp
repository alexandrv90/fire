#include "app/FireView.hpp"

#include "render/Viewport.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>

FireView::FireView(QWidget* const parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::StrongFocus);
}

void FireView::present(const PixelBuffer& pixels) {
    constexpr auto MAXIMUM_IMAGE_DIMENSION = static_cast<std::size_t>(std::numeric_limits<int>::max());
    assert(pixels.width() <= MAXIMUM_IMAGE_DIMENSION && pixels.height() <= MAXIMUM_IMAGE_DIMENSION);
    if (pixels.width() == 0 || pixels.height() == 0 || pixels.width() > MAXIMUM_IMAGE_DIMENSION ||
        pixels.height() > MAXIMUM_IMAGE_DIMENSION) {
        frame = {};
        updateGeometry();
        update();
        return;
    }

    const auto frameWidth = static_cast<int>(pixels.width());
    const auto frameHeight = static_cast<int>(pixels.height());
    if (frame.width() != frameWidth || frame.height() != frameHeight) {
        frame = QImage(frameWidth, frameHeight, QImage::Format_RGB32);
        updateGeometry();
        if (frame.isNull()) {
            update();
            return;
        }
    }

    const std::size_t rowByteCount = pixels.width() * sizeof(Rgba32);
    for (std::size_t y = 0; y < pixels.height(); ++y) {
        const Rgba32* const source = pixels.data() + y * pixels.width();
        std::memcpy(frame.scanLine(static_cast<int>(y)), source, rowByteCount);
    }

    update();
}

QSize FireView::minimumSizeHint() const { return frame.size() / 2; }

QSize FireView::sizeHint() const { return frame.size(); }

void FireView::paintEvent(QPaintEvent* const event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const FitRect fitted = fitPreservingAspect(width(), height(), frame.width(), frame.height());
    painter.drawImage(QRect{fitted.x, fitted.y, fitted.width, fitted.height}, frame);
}
