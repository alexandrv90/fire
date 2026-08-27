#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

// Shared width/height vocabulary. Each consuming domain validates the
// constraints it owns; this type only provides safe common arithmetic.
struct Dimensions final {
    constexpr Dimensions(const std::size_t width, const std::size_t height) noexcept : width(width), height(height) {}

    std::size_t width;
    std::size_t height;

    [[nodiscard]] constexpr bool isEmpty() const noexcept { return width == 0 || height == 0; }

    [[nodiscard]] constexpr bool hasRepresentableArea() const noexcept {
        return width == 0 || height <= std::numeric_limits<std::size_t>::max() / width;
    }

    [[nodiscard]] constexpr std::size_t area() const noexcept {
        assert(hasRepresentableArea());
        return width * height;
    }

    [[nodiscard]] friend constexpr bool operator==(const Dimensions&, const Dimensions&) noexcept = default;
};
