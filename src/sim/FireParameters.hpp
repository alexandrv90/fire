#pragma once

#include <cstdint>

// Tunable inputs to the fire simulation. Every setter clamps, so an instance is
// always within range and can be handed to a UI layer as the authority on both
// current values and permitted bounds.
class FireParameters final {
public:
    static constexpr std::uint8_t MINIMUM_SOURCE_HEAT = 32;
    static constexpr std::uint8_t MAXIMUM_SOURCE_HEAT = 255;
    static constexpr std::uint8_t MINIMUM_COOLING = 1;
    static constexpr std::uint8_t MAXIMUM_COOLING = 8;

    void setSourceHeat(std::uint8_t sourceHeat) noexcept;
    void setCooling(std::uint8_t cooling) noexcept;

    [[nodiscard]] std::uint8_t sourceHeat() const noexcept { return sourceHeatLevel; }
    [[nodiscard]] std::uint8_t cooling() const noexcept { return coolingRate; }

private:
    std::uint8_t sourceHeatLevel{240};
    std::uint8_t coolingRate{4};
};
