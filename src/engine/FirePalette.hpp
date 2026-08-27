#pragma once

#include "engine/PixelBuffer.hpp"

#include <array>
#include <cstdint>
#include <span>
#include <string_view>

struct PaletteStop {
    std::uint8_t index;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

enum class FirePalettePresetId : std::uint8_t {
    Classic,
    Ghostlight,
    ArcaneBloom,
};

struct FirePalettePreset {
    FirePalettePresetId id;
    std::string_view name;
    std::span<const PaletteStop> stops;
};

[[nodiscard]] std::span<const FirePalettePreset> firePalettePresets() noexcept;
[[nodiscard]] const FirePalettePreset& firePalettePreset(FirePalettePresetId id) noexcept;

class FirePalette final {
public:
    [[nodiscard]] static FirePalette classic() noexcept;
    [[nodiscard]] static FirePalette fromPreset(FirePalettePresetId id) noexcept;
    [[nodiscard]] static FirePalette fromStops(std::span<const PaletteStop> stops) noexcept;

    [[nodiscard]] Rgba32 operator[](std::uint8_t heat) const noexcept;

private:
    explicit FirePalette(std::array<Rgba32, 256> colors) noexcept;

    std::array<Rgba32, 256> colors;
};
