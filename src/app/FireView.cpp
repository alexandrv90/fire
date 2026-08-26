#include "app/FireView.hpp"

#include "Utils.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <cstddef>
#include <limits>

namespace {
constexpr std::size_t MAXIMUM_IMAGE_WIDTH = static_cast<std::size_t>(std::numeric_limits<int>::max()) / sizeof(Rgba32);
constexpr std::size_t MAXIMUM_IMAGE_HEIGHT = static_cast<std::size_t>(std::numeric_limits<int>::max());
} // namespace

FireView::FireView(const PixelBuffer& pixels, QWidget* const parent) : QWidget(parent) {
    rewrap(pixels);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::StrongFocus);
}

void FireView::present() { update(); }

void FireView::rewrap(const PixelBuffer& pixels) {
    wrappedData = pixels.data();
    wrappedWidth = pixels.width();
    wrappedHeight = pixels.height();

    if (wrappedWidth == 0 || wrappedHeight == 0 || wrappedWidth > MAXIMUM_IMAGE_WIDTH ||
        wrappedHeight > MAXIMUM_IMAGE_HEIGHT) {
        frame = {};
        updateGeometry();
        return;
    }

    // Rgba32 and QRgb share the 0xAARRGGBB word layout, so the buffer needs no
    // conversion. The const overload yields a read-only image: painting reads it
    // without ever detaching, which would silently deep-copy the aliased storage.
    static_assert(sizeof(Rgba32) == sizeof(QRgb));
    frame = QImage(reinterpret_cast<const uchar*>(wrappedData),
                   static_cast<int>(wrappedWidth),
                   static_cast<int>(wrappedHeight),
                   static_cast<int>(wrappedWidth * sizeof(Rgba32)),
                   QImage::Format_RGB32);
    updateGeometry();
}

QSize FireView::minimumSizeHint() const { return frame.size() / 2; }

QSize FireView::sizeHint() const { return frame.size(); }

void FireView::paintEvent(QPaintEvent* const event) {
    Q_UNUSED(event);

    const auto paintStartedAt = MetricsClock::now();
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const FitRect fitted = fitPreservingAspect(width(), height(), frame.width(), frame.height());
    painter.drawImage(QRect{fitted.x, fitted.y, fitted.width, fitted.height}, frame);

    if (metricsEnabled) {
        emit paintMeasured(paintStartedAt, MetricsClock::now() - paintStartedAt);
    }
}
