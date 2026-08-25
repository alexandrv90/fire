#include "render/FireRenderer.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

FireRenderer::FireRenderer(FirePalette palette) noexcept : palette(std::move(palette)) {}

void FireRenderer::setPalette(const FirePalette& newPalette) noexcept { palette = newPalette; }

void FireRenderer::render(const HeatFrame& heat) {
    if (renderTarget.width() != heat.width() || renderTarget.height() != heat.height()) {
        renderTarget.resize(heat.width(), heat.height());
    }

    for (std::size_t y = 0; y < heat.height(); ++y) {
        const std::span<const std::uint8_t> source = heat.row(y);
        const std::span<Rgba32> destination = renderTarget.row(y);
        for (std::size_t x = 0; x < source.size(); ++x) {
            destination[x] = palette[source[x]];
        }
    }
}
