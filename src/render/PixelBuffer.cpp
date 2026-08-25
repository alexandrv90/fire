#include "render/PixelBuffer.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

void PixelBuffer::resize(const std::size_t width, const std::size_t height) {
    if (width != 0 && height > std::numeric_limits<std::size_t>::max() / width) {
        throw std::length_error("Pixel buffer dimensions are too large");
    }

    pixels.resize(width * height);
    bufferWidth = width;
    bufferHeight = height;
}

std::span<Rgba32> PixelBuffer::row(const std::size_t y) noexcept {
    assert(y < bufferHeight);
    return std::span<Rgba32>{pixels}.subspan(y * bufferWidth, bufferWidth);
}
