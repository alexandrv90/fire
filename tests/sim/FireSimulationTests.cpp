#include "sim/FireSimulation.hpp"
#include "tests_common.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string_view>

namespace {
constexpr std::size_t SIMULATION_WIDTH = 64;
constexpr std::size_t SIMULATION_HEIGHT = 48;
constexpr std::uint32_t RANDOM_SEED = 0x12345678u;
constexpr int TICK_COUNT = 60;

using fire_tests::check;

void checkHash(const std::uint64_t actual, const std::uint64_t expected, const std::string_view message) {
    if (actual == expected) {
        return;
    }

    std::cerr << "FAILED: " << message << " (expected " << expected << ", got " << actual << ")\n";
    fire_tests::recordFailure();
}

template <typename ExpectedException>
void checkInvalidDimensions(const std::size_t width, const std::size_t height, const std::string_view message) {
    fire_tests::checkThrows<ExpectedException>(
        [width, height] { [[maybe_unused]] const FireSimulation simulation{width, height}; }, message);
}

[[nodiscard]] std::uint64_t heatHash(const HeatFrame& heat) noexcept {
    constexpr std::uint64_t FNV_OFFSET_BASIS = 14'695'981'039'346'656'037ull;
    constexpr std::uint64_t FNV_PRIME = 1'099'511'628'211ull;

    std::uint64_t hash = FNV_OFFSET_BASIS;
    for (const std::uint8_t cell : heat.cells()) {
        hash ^= cell;
        hash *= FNV_PRIME;
    }
    return hash;
}

void advance(FireSimulation& simulation, const int tickCount) noexcept {
    for (int tick = 0; tick < tickCount; ++tick) {
        simulation.tick();
    }
}

void testConstruction() {
    const FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    const HeatFrame heat = simulation.heat();

    check(simulation.width() == SIMULATION_WIDTH, "simulation reports its width");
    check(simulation.height() == SIMULATION_HEIGHT, "simulation reports its height");
    check(heat.width() == SIMULATION_WIDTH, "heat frame reports the simulation width");
    check(heat.height() == SIMULATION_HEIGHT, "heat frame reports the simulation height");
    check(heat.cells().size() == SIMULATION_WIDTH * SIMULATION_HEIGHT, "heat frame matches its geometry");
    check(heat.row(0).size() == SIMULATION_WIDTH, "heat frame exposes complete rows");
    check(heat.row(SIMULATION_HEIGHT - 1).data() == heat.cells().data() + (SIMULATION_HEIGHT - 1) * SIMULATION_WIDTH,
          "heat frame locates its final row");

    checkInvalidDimensions<std::invalid_argument>(1, 2, "simulation rejects a width below two");
    checkInvalidDimensions<std::invalid_argument>(2, 1, "simulation rejects a height below two");
    checkInvalidDimensions<std::length_error>(
        std::numeric_limits<std::size_t>::max(), 2, "simulation rejects dimensions whose area cannot be represented");
}

void testDefaultSimulationRegression() {
    constexpr std::uint64_t EXPECTED_HEAT_HASH = 6'789'784'635'805'293'696ull;

    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    advance(simulation, TICK_COUNT);

    checkHash(heatHash(simulation.heat()), EXPECTED_HEAT_HASH, "default simulation heat is stable after sixty ticks");
}

void testParameterizedSimulationRegression() {
    constexpr std::uint64_t EXPECTED_HEAT_HASH = 12'988'646'552'676'957'445ull;

    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    simulation.parameters().setSourceHeat(192);
    simulation.parameters().setCooling(5);
    simulation.reset();
    advance(simulation, TICK_COUNT);

    checkHash(
        heatHash(simulation.heat()), EXPECTED_HEAT_HASH, "parameterized simulation heat is stable after sixty ticks");
}

void testResetReplaysDeterministicSequence() {
    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    const std::uint64_t initialHash = heatHash(simulation.heat());

    advance(simulation, TICK_COUNT);
    const std::uint64_t firstRunHash = heatHash(simulation.heat());

    simulation.reset();
    checkHash(heatHash(simulation.heat()), initialHash, "reset restores the initial heat map");

    advance(simulation, TICK_COUNT);
    checkHash(heatHash(simulation.heat()), firstRunHash, "reset replays the same random sequence");
}
} // namespace

int main() {
    testConstruction();
    testDefaultSimulationRegression();
    testParameterizedSimulationRegression();
    testResetReplaysDeterministicSequence();

    return fire_tests::reportResults("simulation");
}
