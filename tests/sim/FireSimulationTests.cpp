#include "sim/FireSimulation.hpp"
#include "tests_common.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>

namespace {
constexpr std::size_t SIMULATION_WIDTH = 64;
constexpr std::size_t SIMULATION_HEIGHT = 48;
constexpr std::uint32_t RANDOM_SEED = 0x12345678u;
constexpr int TICK_COUNT = 90;

using fire_tests::check;

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

// How high the body of the fire stands, as a share of the field height measured
// from the top: 0 means it reaches the top, 1 means nothing rose off the base.
// Individual tongues run well past this - they are what the cooling map is for -
// so the highest lit cell would say little about the fire as a whole.
[[nodiscard]] double flameReach(const HeatFrame& heat) noexcept {
    for (std::size_t y = 0; y < heat.height(); ++y) {
        std::size_t lit = 0;
        for (const std::uint8_t cell : heat.row(y)) {
            lit += cell != 0 ? 1u : 0u;
        }
        if (lit * 2 >= heat.width()) {
            return static_cast<double>(y) / static_cast<double>(heat.height());
        }
    }
    return 1.0;
}

[[nodiscard]] std::uint8_t hottestInRow(const HeatFrame& heat, const std::size_t y) noexcept {
    std::uint8_t hottest = 0;
    for (const std::uint8_t cell : heat.row(y)) {
        hottest = std::max(hottest, cell);
    }
    return hottest;
}

[[nodiscard]] std::uint8_t coldestInRow(const HeatFrame& heat, const std::size_t y) noexcept {
    std::uint8_t coldest = 255;
    for (const std::uint8_t cell : heat.row(y)) {
        coldest = std::min(coldest, cell);
    }
    return coldest;
}

[[nodiscard]] std::size_t distinctValuesInRow(const HeatFrame& heat, const std::size_t y) noexcept {
    bool seen[256]{};
    std::size_t distinct = 0;
    for (const std::uint8_t cell : heat.row(y)) {
        distinct += seen[cell] ? 0u : 1u;
        seen[cell] = true;
    }
    return distinct;
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
    check(flameReach(heat) == 1.0, "a fresh field is cold above the base");

    checkInvalidDimensions<std::invalid_argument>(1, 2, "simulation rejects a width below two");
    checkInvalidDimensions<std::invalid_argument>(2, 1, "simulation rejects a height below two");
    checkInvalidDimensions<std::length_error>(
        std::numeric_limits<std::size_t>::max(), 2, "simulation rejects dimensions whose area cannot be represented");
}

// A width that is not a multiple of the cooling map's lattice is widened
// internally; nothing of that may reach the caller.
void testUnalignedWidthStaysHidden() {
    constexpr std::size_t UNALIGNED_WIDTH = 65;

    FireSimulation simulation{UNALIGNED_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    advance(simulation, TICK_COUNT);
    const HeatFrame heat = simulation.heat();

    check(simulation.width() == UNALIGNED_WIDTH, "simulation reports the width it was asked for");
    check(heat.cells().size() == UNALIGNED_WIDTH * SIMULATION_HEIGHT, "heat frame matches an unaligned geometry");
    check(flameReach(heat) < 1.0, "an unaligned field still burns");
}

void testFlamesRiseAndBreakUp() {
    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    advance(simulation, TICK_COUNT);
    const HeatFrame heat = simulation.heat();

    const double reach = flameReach(heat);
    check(reach > 0.0 && reach < 0.9, "flames rise well into the field without filling it");

    // The heat source sits in rows below the visible field, so the bottom row is
    // already burning rather than being the fuel itself: hot everywhere, and only
    // as far from uniform as one row of cooling can make it.
    const std::size_t base = SIMULATION_HEIGHT - 1;
    check(coldestInRow(heat, base) > 192, "the visible base is hot across its whole width");
    check(hottestInRow(heat, base) - coldestInRow(heat, base) < 64, "the visible base is close to uniform");

    // The cooling map is the only thing breaking the horizontal symmetry that
    // diffusion restores, so this is what separates tongues from a gradient.
    check(distinctValuesInRow(heat, base - SIMULATION_HEIGHT / 4) > 8, "the cooling map carves tongues above the base");
}

void testCoolingShortensFlames() {
    FireSimulation gentle{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    FireParameters gentleParameters = gentle.parameters();
    gentleParameters.setCooling(FireParameters::MINIMUM_COOLING);
    gentle.setParameters(gentleParameters);
    advance(gentle, TICK_COUNT);

    FireSimulation harsh{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    FireParameters harshParameters = harsh.parameters();
    harshParameters.setCooling(FireParameters::MAXIMUM_COOLING);
    harsh.setParameters(harshParameters);
    advance(harsh, TICK_COUNT);

    check(flameReach(gentle.heat()) < flameReach(harsh.heat()), "heavier cooling holds the flames lower");
}

void testSourceHeatScalesTheBase() {
    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    FireParameters parameters = simulation.parameters();
    parameters.setSourceHeat(FireParameters::MAXIMUM_SOURCE_HEAT);
    simulation.setParameters(parameters);
    simulation.tick();
    const std::uint8_t hotBase = hottestInRow(simulation.heat(), SIMULATION_HEIGHT - 1);

    parameters.setSourceHeat(FireParameters::MINIMUM_SOURCE_HEAT);
    simulation.setParameters(parameters);
    simulation.tick();
    const std::uint8_t coolBase = hottestInRow(simulation.heat(), SIMULATION_HEIGHT - 1);

    check(hotBase > 224, "full source heat drives the base to the top of the palette");
    check(coolBase < hotBase / 4, "lowering the source heat cools the base");
}

void testResetReplaysDeterministicSequence() {
    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    const std::uint64_t initialHash = heatHash(simulation.heat());

    advance(simulation, TICK_COUNT);
    const std::uint64_t firstRunHash = heatHash(simulation.heat());
    check(firstRunHash != initialHash, "ticking changes the heat map");

    simulation.reset();
    check(heatHash(simulation.heat()) == initialHash, "reset restores the initial heat map");

    advance(simulation, TICK_COUNT);
    check(heatHash(simulation.heat()) == firstRunHash, "reset replays the same sequence");
}

void testSeedSelectsTheCoolingMap() {
    FireSimulation first{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    FireSimulation same{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED};
    FireSimulation other{SIMULATION_WIDTH, SIMULATION_HEIGHT, RANDOM_SEED + 1u};

    advance(first, TICK_COUNT);
    advance(same, TICK_COUNT);
    advance(other, TICK_COUNT);

    check(heatHash(first.heat()) == heatHash(same.heat()), "one seed always gives the same fire");
    check(heatHash(first.heat()) != heatHash(other.heat()), "another seed gives another fire");
}
} // namespace

int main() {
    testConstruction();
    testUnalignedWidthStaysHidden();
    testFlamesRiseAndBreakUp();
    testCoolingShortensFlames();
    testSourceHeatScalesTheBase();
    testResetReplaysDeterministicSequence();
    testSeedSelectsTheCoolingMap();

    return fire_tests::reportResults("simulation2");
}
