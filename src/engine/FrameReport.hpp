#pragma once

#include <chrono>
#include <cstdint>

struct FrameReport {
    int ticksExecuted{0};
    std::chrono::steady_clock::duration elapsed{};
    std::chrono::steady_clock::duration discardedTime{};
    std::uint64_t frameIndex{0};
};
