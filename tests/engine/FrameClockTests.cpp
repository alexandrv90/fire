#include "engine/FrameClock.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {
using namespace std::chrono_literals;

int failureCount = 0;

void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    ++failureCount;
}

void checkPlan(const TickPlan& plan,
               const int expectedTicks,
               const std::chrono::steady_clock::duration expectedDiscardedTime,
               const std::string_view message) {
    check(plan.ticks == expectedTicks && plan.discardedTime == expectedDiscardedTime, message);
}

template <typename Action>
void checkInvalidArgument(Action action, const std::string_view message) {
    try {
        action();
    } catch (const std::invalid_argument&) {
        return;
    } catch (const std::exception& exception) {
        std::cerr << "FAILED: " << message << " (unexpected exception: " << exception.what() << ")\n";
        ++failureCount;
        return;
    } catch (...) {
        std::cerr << "FAILED: " << message << " (unexpected non-standard exception)\n";
        ++failureCount;
        return;
    }

    std::cerr << "FAILED: " << message << " (no exception)\n";
    ++failureCount;
}

void testConstructionValidation() {
    checkInvalidArgument([] { [[maybe_unused]] const FrameClock clock{0, 3}; },
                         "frame clock requires a positive tick rate");
    checkInvalidArgument([] { [[maybe_unused]] const FrameClock clock{60, 0}; },
                         "frame clock requires a positive catch-up limit");
}

void testElapsedTimeAccumulates() {
    FrameClock clock{4, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step produces no tick");
    checkPlan(clock.consume(125ms), 1, 0ns, "partial elapsed times accumulate into a tick");
    checkPlan(clock.consume(500ms), 2, 0ns, "one wake can produce multiple ticks");
}

void testCatchUpClampReportsDiscardedTime() {
    FrameClock clock{4, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step is retained before a delayed wake");
    checkPlan(clock.consume(1s), 3, 375ms, "the catch-up clamp reports time it discards");
    checkPlan(clock.consume(0ns), 0, 0ns, "the clamped catch-up plan leaves no hidden backlog");
}

void testProductionTickRateBoundary() {
    FrameClock clock{60, 3};

    checkPlan(clock.consume(16ms), 0, 0ns, "a sixteen millisecond wake is shorter than one 60 Hz step");
    checkPlan(clock.consume(1ms), 1, 0ns, "the next wake drains the accumulated 60 Hz step");
}

void testResetClearsAccumulatedTime() {
    FrameClock clock{4, 3};

    checkPlan(clock.consume(125ms), 0, 0ns, "a partial step exists before reset");
    clock.reset();
    checkPlan(clock.consume(125ms), 0, 0ns, "reset clears partial elapsed time");
    checkPlan(clock.consume(125ms), 1, 0ns, "the clock accumulates normally after reset");
}
} // namespace

int main() {
    testConstructionValidation();
    testElapsedTimeAccumulates();
    testCatchUpClampReportsDiscardedTime();
    testProductionTickRateBoundary();
    testResetClearsAccumulatedTime();

    if (failureCount != 0) {
        std::cerr << failureCount << " frame clock test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All frame clock tests passed\n";
    return 0;
}
