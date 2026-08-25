#pragma once

#include <cstdint>

struct FitRect {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
};

[[nodiscard]] constexpr FitRect fitPreservingAspect(const int availableWidth,
                                                    const int availableHeight,
                                                    const int sourceWidth,
                                                    const int sourceHeight) noexcept {
    if (availableWidth <= 0 || availableHeight <= 0 || sourceWidth <= 0 || sourceHeight <= 0) {
        return {};
    }

    const auto widthLimitedHeight = static_cast<std::int64_t>(availableWidth) * sourceHeight;
    const auto heightLimitedWidth = static_cast<std::int64_t>(availableHeight) * sourceWidth;

    int fittedWidth = availableWidth;
    int fittedHeight = availableHeight;
    if (widthLimitedHeight <= heightLimitedWidth) {
        fittedHeight = static_cast<int>(widthLimitedHeight / sourceWidth);
    } else {
        fittedWidth = static_cast<int>(heightLimitedWidth / sourceHeight);
    }

    return {(availableWidth - fittedWidth) / 2, (availableHeight - fittedHeight) / 2, fittedWidth, fittedHeight};
}
