#include "engine/FrameClock.hpp"

#include <stdexcept>

namespace {
[[nodiscard]] std::chrono::duration<double> tickDurationFor(const int ticksPerSecond) {
    if (ticksPerSecond <= 0) {
        throw std::invalid_argument("ticks per second must be positive");
    }

    return std::chrono::duration<double>{1.0 / static_cast<double>(ticksPerSecond)};
}

[[nodiscard]] int validatedMaximumTicksPerWake(const int maximumTicksPerWake) {
    if (maximumTicksPerWake <= 0) {
        throw std::invalid_argument("maximum ticks per wake must be positive");
    }

    return maximumTicksPerWake;
}
} // namespace

FrameClock::FrameClock(const int ticksPerSecond, const int maximumTicksPerWake)
    : tickDuration(tickDurationFor(ticksPerSecond)),
      maximumTicksPerWake(validatedMaximumTicksPerWake(maximumTicksPerWake)) {}

TickPlan FrameClock::consume(const std::chrono::steady_clock::duration elapsed) noexcept {
    accumulatedTime += elapsed;

    const AccumulatorDuration maximumCatchUpTime = tickDuration * maximumTicksPerWake;
    AccumulatorDuration discardedTime{0.0};
    if (accumulatedTime > maximumCatchUpTime) {
        discardedTime = accumulatedTime - maximumCatchUpTime;
        accumulatedTime = maximumCatchUpTime;
    }

    int ticks = 0;
    while (accumulatedTime >= tickDuration && ticks < maximumTicksPerWake) {
        accumulatedTime -= tickDuration;
        ++ticks;
    }

    return TickPlan{ticks, std::chrono::duration_cast<std::chrono::steady_clock::duration>(discardedTime)};
}

void FrameClock::reset() noexcept { accumulatedTime = AccumulatorDuration{0.0}; }
