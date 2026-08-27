#include "engine/FirePalette.hpp"

#include <array>
#include <cstddef>
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

constexpr std::array GHOSTLIGHT_PALETTE_STOPS{
    PaletteStop{0, 0, 0, 0},
    PaletteStop{40, 0, 8, 18},
    PaletteStop{88, 0, 48, 58},
    PaletteStop{144, 0, 150, 125},
    PaletteStop{200, 90, 240, 170},
    PaletteStop{236, 190, 255, 230},
    PaletteStop{255, 255, 255, 255},
};

constexpr std::array ARCANE_BLOOM_PALETTE_STOPS{
    PaletteStop{0, 0, 0, 0},
    PaletteStop{32, 18, 0, 30},
    PaletteStop{80, 70, 0, 110},
    PaletteStop{136, 180, 0, 190},
    PaletteStop{192, 255, 60, 150},
    PaletteStop{232, 150, 200, 255},
    PaletteStop{255, 255, 255, 255},
};

constexpr std::array PALETTE_PRESETS{
    FirePalettePreset{FirePalettePresetId::Classic, "Classic", CLASSIC_PALETTE_STOPS},
    FirePalettePreset{FirePalettePresetId::Ghostlight, "Ghostlight", GHOSTLIGHT_PALETTE_STOPS},
    FirePalettePreset{FirePalettePresetId::ArcaneBloom, "Arcane Bloom", ARCANE_BLOOM_PALETTE_STOPS},
};

[[nodiscard]] constexpr bool paletteStopsAreValid(const std::span<const PaletteStop> stops) noexcept {
    if (stops.size() < 2 || stops.front().index != 0 || stops.back().index != 255) {
        return false;
    }

    for (std::size_t index = 1; index < stops.size(); ++index) {
        if (stops[index].index <= stops[index - 1].index) {
            return false;
        }
    }

    return true;
}

static_assert(paletteStopsAreValid(CLASSIC_PALETTE_STOPS));
static_assert(paletteStopsAreValid(GHOSTLIGHT_PALETTE_STOPS));
static_assert(paletteStopsAreValid(ARCANE_BLOOM_PALETTE_STOPS));
static_assert(PALETTE_PRESETS.front().id == FirePalettePresetId::Classic);

[[nodiscard]] constexpr Rgba32 makeColor(const int red, const int green, const int blue) noexcept {
    return 0xFF000000u | (static_cast<Rgba32>(red) << 16u) | (static_cast<Rgba32>(green) << 8u) |
           static_cast<Rgba32>(blue);
}
} // namespace

std::span<const FirePalettePreset> firePalettePresets() noexcept { return PALETTE_PRESETS; }

const FirePalettePreset& firePalettePreset(const FirePalettePresetId id) noexcept {
    for (const FirePalettePreset& preset : PALETTE_PRESETS) {
        if (preset.id == id) {
            return preset;
        }
    }

    return PALETTE_PRESETS.front();
}

FirePalette FirePalette::classic() noexcept { return fromStops(CLASSIC_PALETTE_STOPS); }

FirePalette FirePalette::fromPreset(const FirePalettePresetId id) noexcept {
    return fromStops(firePalettePreset(id).stops);
}

FirePalette FirePalette::fromStops(const std::span<const PaletteStop> stops) noexcept {
    const std::span<const PaletteStop> effectiveStops =
        paletteStopsAreValid(stops) ? stops : std::span<const PaletteStop>{CLASSIC_PALETTE_STOPS};

    std::array<Rgba32, 256> colors{};
    for (std::size_t segment = 0; segment < effectiveStops.size() - 1; ++segment) {
        const PaletteStop& first = effectiveStops[segment];
        const PaletteStop& second = effectiveStops[segment + 1];

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
