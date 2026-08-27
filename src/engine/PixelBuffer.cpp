#include "engine/PixelBuffer.hpp"

#include <cassert>
#include <stdexcept>

PixelBuffer::PixelBuffer(const Dimensions dimensions) : bufferDimensions(dimensions) {
    if (bufferDimensions.isEmpty()) {
        throw std::invalid_argument("Pixel buffer dimensions must both be non-zero");
    }
    if (!bufferDimensions.hasRepresentableArea() || bufferDimensions.area() > pixels.max_size()) {
        throw std::length_error("Pixel buffer dimensions are too large");
    }

    pixels.resize(bufferDimensions.area());
}

std::span<Rgba32> PixelBuffer::row(const std::size_t y) noexcept {
    assert(y < bufferDimensions.height);
    return std::span<Rgba32>{pixels}.subspan(y * bufferDimensions.width, bufferDimensions.width);
}
