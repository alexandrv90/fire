#pragma once

#include "sim/Dimensions.hpp"

#include <cassert>
#include <cstdint>
#include <span>

class HeatFrame final {
public:
    constexpr HeatFrame(const std::span<const std::uint8_t> cells, const Dimensions dimensions) noexcept
        : frameCells(cells), frameDimensions(dimensions) {
        assert(frameDimensions.hasRepresentableArea());
        assert(frameCells.size() == frameDimensions.area());
    }

    [[nodiscard]] constexpr Dimensions dimensions() const noexcept { return frameDimensions; }
    [[nodiscard]] constexpr std::size_t width() const noexcept { return frameDimensions.width; }
    [[nodiscard]] constexpr std::size_t height() const noexcept { return frameDimensions.height; }
    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept { return frameCells; }

    [[nodiscard]] constexpr std::span<const std::uint8_t> row(const std::size_t y) const noexcept {
        assert(y < frameDimensions.height);
        return frameCells.subspan(y * frameDimensions.width, frameDimensions.width);
    }

private:
    std::span<const std::uint8_t> frameCells;
    Dimensions frameDimensions;
};
