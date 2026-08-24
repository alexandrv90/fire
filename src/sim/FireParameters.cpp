#include "sim/FireParameters.hpp"

#include <algorithm>

void FireParameters::setSourceHeat(const std::uint8_t sourceHeat) noexcept {
    sourceHeatLevel = std::clamp(sourceHeat, MINIMUM_SOURCE_HEAT, MAXIMUM_SOURCE_HEAT);
}

void FireParameters::setCooling(const std::uint8_t cooling) noexcept {
    coolingRate = std::clamp(cooling, MINIMUM_COOLING, MAXIMUM_COOLING);
}
