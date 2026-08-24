#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

class FireSimulation final {
public:
    static constexpr int MINIMUM_WIND = -8;
    static constexpr int MAXIMUM_WIND = 8;
    static constexpr std::uint8_t MAXIMUM_COOLING = 8;

    explicit FireSimulation(std::size_t width, std::size_t height, std::uint32_t randomSeed = 0xC001CAFEu);

    void tick() noexcept;
    void reset() noexcept;

    [[nodiscard]] std::size_t width() const noexcept { return simulationWidth; }
    [[nodiscard]] std::size_t height() const noexcept { return simulationHeight; }
    [[nodiscard]] std::span<const std::uint8_t> heat() const noexcept { return heatMap; }

    void setSourceHeat(std::uint8_t heat) noexcept { sourceHeatLevel = heat; }
    void setCooling(std::uint8_t cooling) noexcept;
    void setWind(int wind) noexcept;

    [[nodiscard]] std::uint8_t sourceHeat() const noexcept { return sourceHeatLevel; }
    [[nodiscard]] std::uint8_t cooling() const noexcept { return coolingRate; }
    [[nodiscard]] int wind() const noexcept { return windStrength; }

private:
    [[nodiscard]] std::uint32_t nextRandom() noexcept;
    void updateFuelRow() noexcept;

    std::size_t simulationWidth;
    std::size_t simulationHeight;
    std::vector<std::uint8_t> heatMap;
    std::uint32_t initialSeed;
    std::uint32_t randomState;
    std::uint8_t sourceHeatLevel{255};
    std::uint8_t coolingRate{2};
    int windStrength{0};
};
