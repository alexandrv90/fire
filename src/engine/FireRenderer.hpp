#pragma once

#include "engine/FirePalette.hpp"
#include "engine/PixelBuffer.hpp"
#include "sim/HeatFrame.hpp"

class FireRenderer final {
public:
    explicit FireRenderer(FirePalette palette) noexcept;

    void setPalette(const FirePalette& palette) noexcept;
    void render(const HeatFrame& heat, PixelBuffer& target) const noexcept;

private:
    FirePalette palette;
};
