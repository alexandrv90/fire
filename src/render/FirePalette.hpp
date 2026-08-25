#pragma once

#include "render/PixelBuffer.hpp"

#include <array>
#include <cstdint>
#include <span>

struct PaletteStop {
    std::uint8_t index;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

class FirePalette final {
public:
    [[nodiscard]] static FirePalette classic();
    [[nodiscard]] static FirePalette fromStops(std::span<const PaletteStop> stops);

    [[nodiscard]] Rgba32 operator[](std::uint8_t heat) const noexcept;

private:
    explicit FirePalette(std::array<Rgba32, 256> colors) noexcept;

    std::array<Rgba32, 256> colors;
};
