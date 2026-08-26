#include "app/FireView.hpp"

#include "engine/PixelBuffer.hpp"
#include "tests_common.h"

#include <QApplication>
#include <QImage>
#include <QPixmap>
#include <QSize>

#include <algorithm>
#include <cstddef>
#include <span>

namespace {
using fire_tests::check;

constexpr std::size_t BUFFER_WIDTH = 16;
constexpr std::size_t BUFFER_HEIGHT = 16;

void fillBuffer(PixelBuffer& buffer, const Rgba32 color) {
    for (std::size_t y = 0; y < buffer.height(); ++y) {
        const std::span<Rgba32> pixelRow = buffer.row(y);
        std::fill(pixelRow.begin(), pixelRow.end(), color);
    }
}

// Painting a solid buffer at its natural size makes the sampled colour immune to
// both the aspect fit and the smooth scaling filter.
void resizeToNaturalSize(FireView& view) {
    view.resize(static_cast<int>(BUFFER_WIDTH), static_cast<int>(BUFFER_HEIGHT));
}

[[nodiscard]] QRgb paintedCenterPixel(FireView& view) {
    const QImage painted = view.grab().toImage();
    return painted.pixel(painted.width() / 2, painted.height() / 2);
}

void testTracksPresentedBufferGeometry() {
    PixelBuffer buffer;
    buffer.resize(BUFFER_WIDTH, BUFFER_HEIGHT);
    fillBuffer(buffer, 0xFF204080u);
    FireView view{nullptr, buffer};

    check(view.sizeHint() == QSize(static_cast<int>(BUFFER_WIDTH), static_cast<int>(BUFFER_HEIGHT)),
          "the view sizes itself to the presented buffer");
    check(view.minimumSizeHint() == QSize(static_cast<int>(BUFFER_WIDTH) / 2, static_cast<int>(BUFFER_HEIGHT) / 2),
          "the view accepts half the presented buffer size");

    const PixelBuffer emptyBuffer;
    FireView emptyView{nullptr, emptyBuffer};
    check(emptyView.sizeHint() == QSize(0, 0), "constructing with an empty buffer leaves the view without a frame");
}

void testPresentsLiveBufferContents() {
    PixelBuffer buffer;
    buffer.resize(BUFFER_WIDTH, BUFFER_HEIGHT);
    FireView view{nullptr, buffer};
    resizeToNaturalSize(view);

    fillBuffer(buffer, 0xFF0000FFu);
    view.present();
    check(paintedCenterPixel(view) == qRgb(0, 0, 255), "the view paints the presented buffer");

    // Deliberately no second present(): the image aliases the buffer, so a repaint
    // must show the current contents rather than a snapshot taken at present().
    fillBuffer(buffer, 0xFF00FF00u);
    check(paintedCenterPixel(view) == qRgb(0, 255, 0), "the view aliases the buffer instead of copying it");
}
} // namespace

int main(int argc, char* argv[]) {
    QApplication application{argc, argv};
    application.setQuitOnLastWindowClosed(false);

    testTracksPresentedBufferGeometry();
    testPresentsLiveBufferContents();

    return fire_tests::reportResults("fire view");
}
