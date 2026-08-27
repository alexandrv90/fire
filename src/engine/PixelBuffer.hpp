#pragma once

#include "sim/Dimensions.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

using Rgba32 = std::uint32_t;

class PixelBuffer final {
public:
    explicit PixelBuffer(Dimensions dimensions);

    // QImage may alias this storage directly. Preventing resizing and object
    // relocation keeps both the pixel address and its dimensions stable.
    PixelBuffer(const PixelBuffer&) = delete;
    PixelBuffer& operator=(const PixelBuffer&) = delete;
    PixelBuffer(PixelBuffer&&) = delete;
    PixelBuffer& operator=(PixelBuffer&&) = delete;

    [[nodiscard]] Dimensions dimensions() const noexcept { return bufferDimensions; }
    [[nodiscard]] std::span<Rgba32> row(std::size_t y) noexcept;
    [[nodiscard]] const Rgba32* data() const noexcept { return pixels.data(); }

private:
    const Dimensions bufferDimensions;
    std::vector<Rgba32> pixels;
};
