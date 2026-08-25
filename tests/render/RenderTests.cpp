#include "render/FirePalette.hpp"
#include "render/FireRenderer.hpp"
#include "render/PixelBuffer.hpp"
#include "render/Viewport.hpp"
#include "sim/HeatFrame.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string_view>

namespace {
int failureCount = 0;

void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    ++failureCount;
}

void checkColor(const Rgba32 actual, const Rgba32 expected, const std::string_view message) {
    if (actual == expected) {
        return;
    }

    std::cerr << "FAILED: " << message << " (expected 0x" << std::hex << expected << ", got 0x" << actual << std::dec
              << ")\n";
    ++failureCount;
}

template <typename Action>
void checkInvalidArgument(Action action, const std::string_view message) {
    try {
        action();
    } catch (const std::invalid_argument&) {
        return;
    } catch (const std::exception& exception) {
        std::cerr << "FAILED: " << message << " (unexpected exception: " << exception.what() << ")\n";
        ++failureCount;
        return;
    } catch (...) {
        std::cerr << "FAILED: " << message << " (unexpected non-standard exception)\n";
        ++failureCount;
        return;
    }

    std::cerr << "FAILED: " << message << " (no exception)\n";
    ++failureCount;
}

void testPixelBufferGeometry() {
    PixelBuffer buffer;
    check(buffer.width() == 0 && buffer.height() == 0, "a new pixel buffer has empty geometry");

    buffer.resize(3, 2);
    check(buffer.width() == 3, "pixel buffer reports its width");
    check(buffer.height() == 2, "pixel buffer reports its height");
    check(buffer.row(0).size() == 3, "pixel buffer exposes complete rows");
    check(buffer.row(1).data() == buffer.data() + 3, "pixel buffer locates its final row");

    buffer.row(0)[2] = 0xFF010203u;
    checkColor(buffer.data()[2], 0xFF010203u, "pixel buffer rows expose writable storage");

    bool rejectedOversizedGeometry = false;
    try {
        buffer.resize(std::numeric_limits<std::size_t>::max(), 2);
    } catch (const std::length_error&) {
        rejectedOversizedGeometry = true;
    }
    check(rejectedOversizedGeometry, "pixel buffer rejects dimensions whose area cannot be represented");
    check(buffer.width() == 3 && buffer.height() == 2, "a rejected resize preserves pixel buffer geometry");
}

void testClassicPalette() {
    const FirePalette palette = FirePalette::classic();

    checkColor(palette[0], 0xFF000000u, "classic palette begins with black");
    checkColor(palette[32], 0xFF2D0000u, "classic palette retains its first red stop");
    checkColor(palette[136], 0xFFFF2D00u, "classic palette retains its orange stop");
    checkColor(palette[232], 0xFFFFF550u, "classic palette retains its yellow stop");
    checkColor(palette[255], 0xFFFFFFFFu, "classic palette ends with white");
}

void testPaletteInterpolation() {
    constexpr std::array stops{
        PaletteStop{0, 0, 10, 20},
        PaletteStop{2, 10, 20, 30},
        PaletteStop{255, 255, 255, 255},
    };
    const FirePalette palette = FirePalette::fromStops(stops);

    checkColor(palette[0], 0xFF000A14u, "custom palette includes its first stop");
    checkColor(palette[1], 0xFF050F19u, "custom palette linearly interpolates channels");
    checkColor(palette[2], 0xFF0A141Eu, "custom palette includes an interior stop");
    checkColor(palette[255], 0xFFFFFFFFu, "custom palette includes its final stop");
}

void testPaletteValidation() {
    constexpr std::array<PaletteStop, 0> noStops{};
    constexpr std::array missingFirstStop{PaletteStop{1, 0, 0, 0}, PaletteStop{255, 255, 255, 255}};
    constexpr std::array missingLastStop{PaletteStop{0, 0, 0, 0}, PaletteStop{254, 255, 255, 255}};
    constexpr std::array duplicateStop{
        PaletteStop{0, 0, 0, 0},
        PaletteStop{20, 20, 20, 20},
        PaletteStop{20, 30, 30, 30},
        PaletteStop{255, 255, 255, 255},
    };
    constexpr std::array descendingStops{
        PaletteStop{0, 0, 0, 0},
        PaletteStop{30, 30, 30, 30},
        PaletteStop{20, 20, 20, 20},
        PaletteStop{255, 255, 255, 255},
    };

    checkInvalidArgument([&] { static_cast<void>(FirePalette::fromStops(noStops)); },
                         "palette rejects an empty stop list");
    checkInvalidArgument([&] { static_cast<void>(FirePalette::fromStops(missingFirstStop)); },
                         "palette requires its first stop at index zero");
    checkInvalidArgument([&] { static_cast<void>(FirePalette::fromStops(missingLastStop)); },
                         "palette requires its final stop at index 255");
    checkInvalidArgument([&] { static_cast<void>(FirePalette::fromStops(duplicateStop)); },
                         "palette rejects duplicate stop indices");
    checkInvalidArgument([&] { static_cast<void>(FirePalette::fromStops(descendingStops)); },
                         "palette rejects descending stop indices");
}

void testRenderer() {
    constexpr std::array heatCells{
        std::uint8_t{0},
        std::uint8_t{1},
        std::uint8_t{2},
        std::uint8_t{255},
    };
    constexpr std::array paletteStops{
        PaletteStop{0, 0, 0, 0},
        PaletteStop{255, 255, 0, 255},
    };
    FireRenderer renderer{FirePalette::fromStops(paletteStops)};

    renderer.render(HeatFrame{heatCells, 2, 2});
    const PixelBuffer& target = renderer.target();
    check(target.width() == 2 && target.height() == 2, "renderer sizes its target from the heat frame");
    checkColor(target.data()[0], 0xFF000000u, "renderer shades zero heat");
    checkColor(target.data()[1], 0xFF010001u, "renderer shades low heat");
    checkColor(target.data()[2], 0xFF020002u, "renderer shades heat through its palette");
    checkColor(target.data()[3], 0xFFFF00FFu, "renderer shades maximum heat");

    const Rgba32* const originalStorage = target.data();
    renderer.render(HeatFrame{heatCells, 2, 2});
    check(renderer.target().data() == originalStorage, "renderer reuses storage when frame geometry is unchanged");

    constexpr std::array replacementStops{
        PaletteStop{0, 255, 255, 255},
        PaletteStop{255, 0, 0, 0},
    };
    renderer.setPalette(FirePalette::fromStops(replacementStops));
    renderer.render(HeatFrame{heatCells, 4, 1});
    check(renderer.target().width() == 4 && renderer.target().height() == 1,
          "renderer resizes its target when frame geometry changes");
    checkColor(renderer.target().data()[0], 0xFFFFFFFFu, "renderer uses a replacement palette");
    checkColor(renderer.target().data()[3], 0xFF000000u, "replacement palette covers maximum heat");
}

void checkRect(const FitRect& actual, const FitRect& expected, const std::string_view message) {
    check(actual.x == expected.x && actual.y == expected.y && actual.width == expected.width &&
              actual.height == expected.height,
          message);
}

void testViewport() {
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
} // namespace

int main() {
    testPixelBufferGeometry();
    testClassicPalette();
    testPaletteInterpolation();
    testPaletteValidation();
    testRenderer();
    testViewport();

    if (failureCount != 0) {
        std::cerr << failureCount << " render test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All render tests passed\n";
    return 0;
}
