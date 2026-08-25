#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

class HeatFrame final {
public:
    constexpr HeatFrame(const std::span<const std::uint8_t> cells,
                        const std::size_t width,
                        const std::size_t height) noexcept
        : frameCells(cells), frameWidth(width), frameHeight(height) {
        assert(frameWidth == 0 || frameHeight <= std::numeric_limits<std::size_t>::max() / frameWidth);
        assert(frameCells.size() == frameWidth * frameHeight);
    }

    [[nodiscard]] constexpr std::size_t width() const noexcept { return frameWidth; }
    [[nodiscard]] constexpr std::size_t height() const noexcept { return frameHeight; }
    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept { return frameCells; }

    [[nodiscard]] constexpr std::span<const std::uint8_t> row(const std::size_t y) const noexcept {
        assert(y < frameHeight);
        return frameCells.subspan(y * frameWidth, frameWidth);
    }

private:
    std::span<const std::uint8_t> frameCells;
    std::size_t frameWidth;
    std::size_t frameHeight;
};
