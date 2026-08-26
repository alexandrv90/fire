#include "engine/FrameClock.hpp"

FrameClock::FrameClock(const std::chrono::duration<double> tickDuration, const int maximumTicksPerWake)
    : tickDuration(tickDuration), maximumTicksPerWake(maximumTicksPerWake) {}

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
