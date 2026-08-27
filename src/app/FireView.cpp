#include "app/FireView.hpp"

#include "app/AspectFit.hpp"
#include "engine/FireEngine.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QSizePolicy>

#include <cstddef>
#include <limits>
#include <stdexcept>

namespace {
constexpr std::size_t MAXIMUM_IMAGE_WIDTH = static_cast<std::size_t>(std::numeric_limits<int>::max()) / sizeof(Rgba32);
constexpr std::size_t MAXIMUM_IMAGE_HEIGHT = static_cast<std::size_t>(std::numeric_limits<int>::max());

constexpr Dimensions DEFAULT_DIMENSIONS = FireEngine::defaultDimensions();
static_assert(DEFAULT_DIMENSIONS.width <= MAXIMUM_IMAGE_WIDTH);
static_assert(DEFAULT_DIMENSIONS.height <= MAXIMUM_IMAGE_HEIGHT);

[[nodiscard]] QImage wrapFrame(const PixelBuffer& pixels) {
    const Dimensions dimensions = pixels.dimensions();
    if (dimensions.width > MAXIMUM_IMAGE_WIDTH || dimensions.height > MAXIMUM_IMAGE_HEIGHT) {
        throw std::length_error("Pixel buffer dimensions cannot be represented by QImage");
    }

    // Rgba32 and QRgb share the 0xAARRGGBB word layout, so the buffer needs no
    // conversion. PixelBuffer guarantees stable geometry and storage, while the
    // composition root guarantees that its owner outlives this borrowing view.
    static_assert(sizeof(Rgba32) == sizeof(QRgb));
    return {reinterpret_cast<const uchar*>(pixels.data()),
            static_cast<int>(dimensions.width),
            static_cast<int>(dimensions.height),
            static_cast<int>(dimensions.width * sizeof(Rgba32)),
            QImage::Format_RGB32};
}
} // namespace

FireView::FireView(const PixelBuffer& pixels, QWidget* const parent) : QWidget(parent), frame(wrapFrame(pixels)) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

void FireView::present() { update(); }

void FireView::paintEvent(QPaintEvent* const event) {
    const auto paintStartedAt = MetricsClock::now();
    const bool paintedWholeView = event->rect() == rect();

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const FitRect fitted = fitPreservingAspect(width(), height(), frame.width(), frame.height());
    painter.drawImage(QRect{fitted.x, fitted.y, fitted.width, fitted.height}, frame);

    if (metricsEnabled && paintedWholeView) {
        emit paintMeasured(paintStartedAt, MetricsClock::now() - paintStartedAt);
    }
}
