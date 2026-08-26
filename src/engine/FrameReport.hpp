#pragma once

#include <chrono>
#include <cstdint>
#include <optional>

struct FrameStageTimings {
    std::chrono::steady_clock::duration simulateDuration{};
    std::chrono::steady_clock::duration shadeDuration{};
};

struct FrameReport {
    int ticksExecuted{0};
    std::chrono::steady_clock::duration elapsed{};
    std::chrono::steady_clock::duration discardedTime{};
    std::uint64_t frameIndex{0};
    std::optional<FrameStageTimings> stageTimings;
};
