#include "app/FireView.hpp"

#include "app/AspectFit.hpp"
#include "app/FireController.hpp"
#include "app/FrameMetricsCollector.hpp"
#include "app/MainWindow.hpp"
#include "app/StatsPanel.hpp"
#include "engine/PixelBuffer.hpp"
#include "tests_common.h"

#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QImage>
#include <QPixmap>
#include <QSize>
#include <QSizePolicy>
#include <QWidget>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <span>
#include <string_view>

namespace {
using fire_tests::check;

constexpr std::size_t BUFFER_WIDTH = 16;
constexpr std::size_t BUFFER_HEIGHT = 16;

void fillBuffer(PixelBuffer& buffer, const Rgba32 color) {
    for (std::size_t y = 0; y < buffer.dimensions().height; ++y) {
        const std::span<Rgba32> pixelRow = buffer.row(y);
        std::fill(pixelRow.begin(), pixelRow.end(), color);
    }
}

void checkRect(const FitRect& actual, const FitRect& expected, const std::string_view message) {
    check(actual.x == expected.x && actual.y == expected.y && actual.width == expected.width &&
              actual.height == expected.height,
          message);
}

void testAspectFit() {
    checkRect(fitPreservingAspect(800, 600, 640, 480), {0, 0, 800, 600}, "viewport preserves a matching aspect");
    checkRect(fitPreservingAspect(100, 100, 16, 9), {0, 22, 100, 56}, "viewport letterboxes a wide source");
    checkRect(fitPreservingAspect(100, 100, 9, 16), {22, 0, 56, 100}, "viewport pillarboxes a tall source");
    checkRect(fitPreservingAspect(101, 100, 2, 1), {0, 25, 101, 50}, "viewport centres an odd-sized fit");
    checkRect(fitPreservingAspect(0, 100, 16, 9), {}, "viewport rejects an empty available width");
    checkRect(fitPreservingAspect(100, 100, 0, 9), {}, "viewport rejects an empty source width");
    checkRect(fitPreservingAspect(-1, 100, 16, 9), {}, "viewport rejects negative dimensions");
    checkRect(fitPreservingAspect(std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max()),
              {0, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max()},
              "viewport handles maximum dimensions without overflow");
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
    PixelBuffer buffer{{BUFFER_WIDTH, BUFFER_HEIGHT}};
    fillBuffer(buffer, 0xFF204080u);
    FireView view{buffer};

    check(view.sizePolicy().horizontalPolicy() == QSizePolicy::Ignored &&
              view.sizePolicy().verticalPolicy() == QSizePolicy::Ignored,
          "the view leaves presentation dimensions to its layout");
    view.resize(8, 24);
    check(view.size() == QSize(8, 24), "presentation dimensions are independent of frame geometry");
}

void testPresentsLiveBufferContents() {
    PixelBuffer buffer{{BUFFER_WIDTH, BUFFER_HEIGHT}};
    FireView view{buffer};
    resizeToNaturalSize(view);

    fillBuffer(buffer, 0xFF0000FFu);
    view.present();
    check(paintedCenterPixel(view) == qRgb(0, 0, 255), "the view paints the presented buffer");

    // Deliberately no second present(): the image aliases the buffer, so a repaint
    // must show the current contents rather than a snapshot taken at present().
    fillBuffer(buffer, 0xFF00FF00u);
    check(paintedCenterPixel(view) == qRgb(0, 255, 0), "the view aliases the buffer instead of copying it");
}

void testQtExclusivelyOwnsWindowView() {
    FrameMetricsCollector metricsCollector;
    FireController controller{{8, 6}};
    MainWindow window{controller, metricsCollector};

    window.setCentralWidget(new QWidget);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

    check(window.findChild<FireView*>() == nullptr,
          "replacing the central widget safely destroys the exclusively Qt-owned fire view");

    // The window keeps observing widgets the tree may already have destroyed.
    QEvent windowStateChange{QEvent::WindowStateChange};
    QCoreApplication::sendEvent(&window, &windowStateChange);
    check(window.findChild<StatsPanel*>() == nullptr,
          "a window state change survives the destruction of the observed stats panel");
}
} // namespace

int main(int argc, char* argv[]) {
    QApplication application{argc, argv};
    application.setQuitOnLastWindowClosed(false);

    testAspectFit();
    testTracksPresentedBufferGeometry();
    testPresentsLiveBufferContents();
    testQtExclusivelyOwnsWindowView();

    return fire_tests::reportResults("fire view");
}
