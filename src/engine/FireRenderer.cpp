#include "engine/FireRenderer.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>
#include <utility>

FireRenderer::FireRenderer(FirePalette palette) noexcept : palette(std::move(palette)) {}

void FireRenderer::setPalette(const FirePalette& newPalette) noexcept { palette = newPalette; }

void FireRenderer::render(const HeatFrame& heat, PixelBuffer& target) const noexcept {
    for (std::size_t y = 0; y < heat.height(); ++y) {
        const std::span<const std::uint8_t> source = heat.row(y);
        const std::span<Rgba32> destination = target.row(y);
        for (std::size_t x = 0; x < source.size(); ++x) {
            destination[x] = palette[source[x]];
        }
    }
}
