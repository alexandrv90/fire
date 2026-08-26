#pragma once

#include <chrono>

struct TickPlan {
    int ticks{0};
    std::chrono::steady_clock::duration discardedTime{};
};

class FrameClock final {
public:
    FrameClock(std::chrono::duration<double> tickDuration, int maximumTicksPerWake);

    [[nodiscard]] TickPlan consume(std::chrono::steady_clock::duration elapsed) noexcept;
    void reset() noexcept;

private:
    using AccumulatorDuration = std::chrono::duration<double>;

    AccumulatorDuration tickDuration;
    int maximumTicksPerWake;
    AccumulatorDuration accumulatedTime{0.0};
};
