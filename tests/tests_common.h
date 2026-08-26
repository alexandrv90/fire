#pragma once

#include <cmath>
#include <exception>
#include <iostream>
#include <string_view>
#include <utility>

namespace fire_tests {
inline int failureCount = 0;

inline void recordFailure() noexcept { ++failureCount; }

inline void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    recordFailure();
}

inline void checkNear(const double actual, const double expected, const std::string_view message) {
    constexpr double TOLERANCE = 1e-9;
    check(std::abs(actual - expected) <= TOLERANCE, message);
}

template <typename ExpectedException, typename Action>
void checkThrows(Action&& action, const std::string_view message) {
    try {
        std::forward<Action>(action)();
    } catch (const ExpectedException&) {
        return;
    } catch (const std::exception& exception) {
        std::cerr << "FAILED: " << message << " (unexpected exception: " << exception.what() << ")\n";
        recordFailure();
        return;
    } catch (...) {
        std::cerr << "FAILED: " << message << " (unexpected non-standard exception)\n";
        recordFailure();
        return;
    }

    std::cerr << "FAILED: " << message << " (no exception)\n";
    recordFailure();
}

inline int reportResults(const std::string_view suiteName) {
    if (failureCount != 0) {
        std::cerr << failureCount << ' ' << suiteName << " test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All " << suiteName << " tests passed\n";
    return 0;
}
} // namespace fire_tests
