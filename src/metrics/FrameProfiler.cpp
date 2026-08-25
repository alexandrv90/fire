#include "metrics/FrameProfiler.hpp"

void FrameProfiler::clear() noexcept {
    simulate.clear();
    shade.clear();
    wakeInterval.clear();
    presentInterval.clear();
}
