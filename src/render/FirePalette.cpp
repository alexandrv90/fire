#include "render/FirePalette.hpp"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace {
constexpr std::array CLASSIC_PALETTE_STOPS{
    PaletteStop{0, 0, 0, 0},
    PaletteStop{32, 45, 0, 0},
    PaletteStop{80, 150, 0, 0},
    PaletteStop{136, 255, 45, 0},
    PaletteStop{192, 255, 160, 0},
    PaletteStop{232, 255, 245, 80},
    PaletteStop{255, 255, 255, 255},
};

[[nodiscard]] constexpr Rgba32 makeColor(const int red, const int green, const int blue) noexcept {
    return 0xFF000000u | (static_cast<Rgba32>(red) << 16u) | (static_cast<Rgba32>(green) << 8u) |
           static_cast<Rgba32>(blue);
}
} // namespace

FirePalette FirePalette::classic() { return fromStops(CLASSIC_PALETTE_STOPS); }

FirePalette FirePalette::fromStops(const std::span<const PaletteStop> stops) {
    if (stops.size() < 2 || stops.front().index != 0 || stops.back().index != 255) {
        throw std::invalid_argument("A palette must have at least two stops spanning indices 0 through 255");
    }

    std::array<Rgba32, 256> colors{};
    for (std::size_t segment = 0; segment + 1 < stops.size(); ++segment) {
        const PaletteStop& first = stops[segment];
        const PaletteStop& second = stops[segment + 1];
        if (second.index <= first.index) {
            throw std::invalid_argument("Palette stop indices must be strictly increasing");
        }

        const int firstIndex = first.index;
        const int secondIndex = second.index;
        const int distance = secondIndex - firstIndex;
        for (int index = firstIndex; index <= secondIndex; ++index) {
            const int offset = index - firstIndex;
            const int red = first.red + (static_cast<int>(second.red) - first.red) * offset / distance;
            const int green = first.green + (static_cast<int>(second.green) - first.green) * offset / distance;
            const int blue = first.blue + (static_cast<int>(second.blue) - first.blue) * offset / distance;
            colors[static_cast<std::size_t>(index)] = makeColor(red, green, blue);
        }
    }

    return FirePalette{std::move(colors)};
}

Rgba32 FirePalette::operator[](const std::uint8_t heat) const noexcept { return colors[heat]; }

FirePalette::FirePalette(std::array<Rgba32, 256> colors) noexcept : colors(std::move(colors)) {}
