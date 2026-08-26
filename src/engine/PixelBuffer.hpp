#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

using Rgba32 = std::uint32_t;

class PixelBuffer final {
public:
    void resize(std::size_t width, std::size_t height);

    [[nodiscard]] std::size_t width() const noexcept { return bufferWidth; }
    [[nodiscard]] std::size_t height() const noexcept { return bufferHeight; }
    [[nodiscard]] std::span<Rgba32> row(std::size_t y) noexcept;
    [[nodiscard]] const Rgba32* data() const noexcept { return pixels.data(); }

private:
    std::size_t bufferWidth{0};
    std::size_t bufferHeight{0};
    std::vector<Rgba32> pixels;
};
