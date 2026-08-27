#include "engine/FirePalette.hpp"
#include "engine/FireRenderer.hpp"
#include "engine/PixelBuffer.hpp"
#include "sim/Dimensions.hpp"
#include "sim/HeatFrame.hpp"
#include "tests_common.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace {
using fire_tests::check;

void checkColor(const Rgba32 actual, const Rgba32 expected, const std::string_view message) {
    if (actual == expected) {
        return;
    }

    std::cerr << "FAILED: " << message << " (expected 0x" << std::hex << expected << ", got 0x" << actual << std::dec
              << ")\n";
    fire_tests::recordFailure();
}

void checkPaletteMatches(const FirePalette& actual, const FirePalette& expected, const std::string_view message) {
    for (std::size_t index = 0; index < 256; ++index) {
        if (actual[static_cast<std::uint8_t>(index)] != expected[static_cast<std::uint8_t>(index)]) {
            check(false, message);
            return;
        }
    }

    check(true, message);
}

void testPixelBufferGeometry() {
    static_assert(!std::is_default_constructible_v<Dimensions>);

    PixelBuffer buffer{{3, 2}};
    check(buffer.dimensions() == Dimensions{3, 2}, "pixel buffer reports its dimensions");
    check(buffer.row(0).size() == 3, "pixel buffer exposes complete rows");
    check(buffer.row(1).data() == buffer.data() + 3, "pixel buffer locates its final row");

    buffer.row(0)[2] = 0xFF010203u;
    checkColor(buffer.data()[2], 0xFF010203u, "pixel buffer rows expose writable storage");

    fire_tests::checkThrows<std::length_error>(
        [] { [[maybe_unused]] const PixelBuffer oversized{{std::numeric_limits<std::size_t>::max(), 2}}; },
        "pixel buffer rejects dimensions whose area cannot be represented");
    fire_tests::checkThrows<std::invalid_argument>([] { [[maybe_unused]] const PixelBuffer empty{{0, 0}}; },
                                                   "pixel buffer rejects empty dimensions");

    static_assert(!std::is_move_constructible_v<PixelBuffer>);
    static_assert(!std::is_move_assignable_v<PixelBuffer>);
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

    static_assert(noexcept(FirePalette::fromStops(noStops)));
    static_assert(noexcept(FirePalette::fromPreset(FirePalettePresetId::Classic)));

    const FirePalette classic = FirePalette::classic();
    checkPaletteMatches(FirePalette::fromStops(noStops), classic, "an empty stop list falls back to classic");
    checkPaletteMatches(
        FirePalette::fromStops(missingFirstStop), classic, "a missing first stop falls back to classic");
    checkPaletteMatches(FirePalette::fromStops(missingLastStop), classic, "a missing final stop falls back to classic");
    checkPaletteMatches(FirePalette::fromStops(duplicateStop), classic, "duplicate stop indices fall back to classic");
    checkPaletteMatches(
        FirePalette::fromStops(descendingStops), classic, "descending stop indices fall back to classic");
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
    PixelBuffer target{{2, 2}};

    renderer.render(HeatFrame{heatCells, {2, 2}}, target);
    check(target.dimensions() == Dimensions{2, 2}, "renderer preserves its caller-owned target geometry");
    checkColor(target.data()[0], 0xFF000000u, "renderer shades zero heat");
    checkColor(target.data()[1], 0xFF010001u, "renderer shades low heat");
    checkColor(target.data()[2], 0xFF020002u, "renderer shades heat through its palette");
    checkColor(target.data()[3], 0xFFFF00FFu, "renderer shades maximum heat");

    const Rgba32* const originalStorage = target.data();
    renderer.render(HeatFrame{heatCells, {2, 2}}, target);
    check(target.data() == originalStorage, "renderer does not replace caller-owned frame storage");

    constexpr std::array replacementStops{
        PaletteStop{0, 255, 255, 255},
        PaletteStop{255, 0, 0, 0},
    };
    renderer.setPalette(FirePalette::fromStops(replacementStops));
    renderer.render(HeatFrame{heatCells, {2, 2}}, target);
    check(target.data() == originalStorage, "switching palettes preserves caller-owned frame storage");
    checkColor(target.data()[0], 0xFFFFFFFFu, "renderer uses a replacement palette");
    checkColor(target.data()[3], 0xFF000000u, "replacement palette covers maximum heat");
}

} // namespace

int main() {
    testPixelBufferGeometry();
    testClassicPalette();
    testPaletteInterpolation();
    testPaletteValidation();
    testRenderer();

    return fire_tests::reportResults("render");
}
