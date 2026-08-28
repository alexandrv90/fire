# Codebase architecture review

## 👤 User  
*2026-08-26 14:34:06.317 UTC*

Go over the existing codebase and review its architecture. This decision should not based on performance evaluation or build artifacts, pure architecture overview. CPU rendering is a conscious choice.

Check how reasonable it when it comes to real-time graphics best practices, what are the weak and strong parts.
Check for speculative code
Check for overengineered abstractions

## 🤖 Assistant  
*2026-08-26 14:34:08.101 UTC*

I'll start by mapping out the codebase structure.

**🔧 Tool call — `Bash`**

*List source files*

```bash
find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.c" -o -name "*.txt" -o -name "*.md" -o -name "*.cmake" \) -not -path "./build/*" -not -path "./.git/*" -not -path "./third_party/*" -not -path "./external/*" | head -100
```

<details>
<summary>Result</summary>

```
./CMakeLists.txt
./README.md
./AGENTS.md
./cmake/ProjectSettings.cmake
./cmake/FireTargets.cmake
./cmake/Sanitizers.cmake
./tests/tests_common.h
./src/main.cpp
./tests/metrics/MetricsTests.cpp
./tests/app/StatsPanelTests.cpp
./tests/app/FireControllerTests.cpp
./tests/app/ControlPanelTests.cpp
./tests/app/FrameMetricsCollectorTests.cpp
./tests/render/RenderTests.cpp
./tests/sim/FireSimulationTests.cpp
./tests/engine/FrameClockTests.cpp
./src/metrics/CMakeLists.txt
./src/metrics/IntervalMetric.hpp
./src/metrics/TimeSeriesMetric.cpp
./src/metrics/MetricStatistics.hpp
./src/metrics/IntervalMetric.cpp
./src/metrics/MetricsClock.hpp
./src/metrics/TimeSeriesMetric.hpp
./src/app/CMakeLists.txt
./src/app/FireController.cpp
./src/app/MainWindow.hpp
./src/app/FrameMetricsCollector.hpp
./src/app/FireView.hpp
./src/app/StatsPanel.hpp
./src/app/ControlPanel.hpp
./src/app/MainWindow.cpp
./src/app/FireController.hpp
./src/app/FrameMetricsCollector.cpp
./src/app/FireView.cpp
./src/app/StatsPanel.cpp
./src/app/ControlPanel.cpp
./src/render/Viewport.hpp
./src/render/CMakeLists.txt
./src/render/FireRenderer.cpp
./src/render/FirePalette.cpp
./src/render/PixelBuffer.cpp
./src/render/FirePalette.hpp
./src/render/PixelBuffer.hpp
./src/render/FireRenderer.hpp
./src/sim/CMakeLists.txt
./src/sim/HeatFrame.hpp
./src/sim/FireParameters.hpp
./src/sim/FireSimulation.cpp
./src/sim/FireParameters.cpp
./src/sim/FireSimulation.hpp
./src/engine/CMakeLists.txt
./src/engine/FireEngine.hpp
./src/engine/FrameClock.cpp
./src/engine/FrameClock.hpp
./src/engine/FrameReport.hpp
./src/engine/FireEngine.cpp
```

</details>

**🔧 Tool call — `Bash`**

*Count lines per file*

```bash
wc -l $(find src tests cmake -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cmake" -o -name "CMakeLists.txt" \)) CMakeLists.txt README.md AGENTS.md | sort -n
```

<details>
<summary>Result</summary>

```
       5 src/metrics/MetricsClock.hpp
      10 src/metrics/MetricStatistics.hpp
      11 src/sim/FireParameters.cpp
      12 CMakeLists.txt
      15 src/sim/CMakeLists.txt
      16 src/metrics/CMakeLists.txt
      17 src/main.cpp
      17 src/metrics/IntervalMetric.hpp
      18 src/engine/CMakeLists.txt
      18 src/engine/FrameReport.hpp
      18 src/metrics/IntervalMetric.cpp
      19 src/app/MainWindow.hpp
      19 src/render/CMakeLists.txt
      19 src/render/FireRenderer.hpp
      20 src/render/PixelBuffer.cpp
      22 src/metrics/TimeSeriesMetric.hpp
      23 src/engine/FrameClock.hpp
      23 src/render/PixelBuffer.hpp
      24 src/render/FireRenderer.cpp
      24 src/sim/FireParameters.hpp
      27 src/render/FirePalette.hpp
      30 src/app/StatsPanel.hpp
      31 cmake/ProjectSettings.cmake
      32 src/app/FireView.hpp
      32 src/render/Viewport.hpp
      32 src/sim/HeatFrame.hpp
      35 src/app/ControlPanel.hpp
      36 src/sim/FireSimulation.hpp
      41 src/engine/FireEngine.hpp
      44 README.md
      44 src/metrics/TimeSeriesMetric.cpp
      46 src/engine/FrameClock.cpp
      48 cmake/Sanitizers.cmake
      49 src/engine/FireEngine.cpp
      51 src/app/FrameMetricsCollector.hpp
      54 src/app/FireController.hpp
      57 src/render/FirePalette.cpp
      57 tests/tests_common.h
      66 AGENTS.md
      68 src/app/FrameMetricsCollector.cpp
      69 src/app/FireView.cpp
      74 tests/app/FireControllerTests.cpp
      82 src/sim/FireSimulation.cpp
      88 tests/metrics/MetricsTests.cpp
      96 src/app/FireController.cpp
      97 cmake/FireTargets.cmake
      98 src/app/CMakeLists.txt
     101 src/app/ControlPanel.cpp
     101 src/app/MainWindow.cpp
     115 tests/sim/FireSimulationTests.cpp
     116 tests/app/ControlPanelTests.cpp
     116 tests/app/FrameMetricsCollectorTests.cpp
     135 tests/app/StatsPanelTests.cpp
     144 src/app/StatsPanel.cpp
     145 tests/engine/FrameClockTests.cpp
     174 tests/render/RenderTests.cpp
    2981 total
```

</details>

**🔧 Tool call — `Bash`**

*Read docs and main*

```bash
for f in README.md AGENTS.md CMakeLists.txt src/main.cpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

````
=========== README.md ===========
# Fire Demo

A C++20 and Qt Widgets demo of the classic procedural fire effect, with live simulation controls.

## Prerequisites

- CMake 3.25+
- Ninja
- Qt 5 or Qt 6 with Widgets
- A C++20 compiler

## Build and run

```sh
cmake --preset cfg-release
cmake --build --preset build-release --target run_app
```

### Optional: build with `just` tool if available

```sh
just run-release
```

## Sanitizer build

Clang and GCC builds can enable AddressSanitizer and UndefinedBehaviorSanitizer together. The sanitizer configuration instruments both the simulation library and the Qt application code; installed Qt libraries are not rebuilt with instrumentation.

```sh
cmake --preset cfg-sanitize
cmake --build --preset build-sanitize

ASAN_OPTIONS=halt_on_error=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
cmake --build --preset build-sanitize --target run_app
```

With `just` installed, configure, build, and run the sanitizer build with:

```sh
just run-sanitize
```

Sanitizers only check code paths exercised while the application runs. Treat any sanitizer diagnostic as a failure, fix the first project-owned stack frame, then rebuild and rerun. Sanitized executables are diagnostic artifacts and should not be distributed as release builds.
=========== AGENTS.md ===========
# AGENTS.md

Operational guide for LLM coding tools working in this repository. Read this first.

If a rule conflicts with a task, stop and surface it. Do not silently work around the rules.

## Acknowledgment

To confirm that you have read necessary files and will respect discovered rules for the duration of the task, print the following in the beggining of each session:

> 🤝 Following project rules.

If you cannot honor a rule for any reason, surface that explicitly instead of printing the acknowledgment.

---

## Doc map

Read these before touching code in the corresponding area.

| If you're working on…                                          | Read first                                        |
| -------------------------------------------------------------- | ------------------------------------------------- |
| Anything                                                       | `README.md`                                       |

---

## Coding rules

Authoritative coding rules for current project. Apply to all contributors — human and LLM.

1. C++20 baseline
2. **Naming.** `lowerCamelCase` for variables and functions, `PascalCase` for types, `SCREAMING_SNAKE_CASE` for constants, `PascalCase` for filenames matching the primary type
3. **Simulation boundary.** The simulation library must remain pure C++; no Qt type may ever cross this boundary.

---

## Implementation approach

- Treat each task as an incremental change to the whole system, not as an isolated patch. Inspect the surrounding code, documented architecture, ownership model and likely extension points before choosing an interface or class design.
- Prefer the smallest coherent design over the smallest diff. Keep the implementation limited to the task, but do not sacrifice clean boundaries, accurate abstractions or future compatibility to finish it quickly.
- Prevent duplication. Search for existing implementations and shared concepts before adding code. When functionality is inaccessible or poorly placed, make a focused refactoring that creates an appropriate shared abstraction instead of copying it.
- Follow modern C++ best practices: use RAII, explicit ownership, value semantics, strong types and standard-library facilities. Avoid owning raw pointers and manual resource cleanup.
- Encapsulate C libraries and other low-level APIs behind narrow C++ abstractions. Resource ownership alone is not sufficient: the abstraction should also enforce valid lifecycle transitions, preserve invariants and keep implementation details from leaking into callers.
- Match documented interfaces and responsibilities by behavior, not merely by class name or file location. Do not introduce a type whose API conflicts with the architectural role its name implies.
- Define error and partial-success semantics deliberately. Avoid ad hoc combinations of status flags, strings and side-channel error collections when a clearer result model is possible.
- Before implementation, identify existing reusable code, resource ownership, object lifecycle and error propagation. After implementation, review the change for duplication, unnecessary repeated work and architectural drift.
- Do not over-engineer speculative features. Broader design awareness should improve the current change without introducing unrelated frameworks or abstractions.

---

## Formatting (end of task)

After all code changes are complete and verified (build/tests pass), run **clang-format** on every C/C++ source and header file you modified in the task. Do this as the **last** step before declaring the work done — not at the start, and not in the middle of edits you still intend to change.

- Use the repo config: `.clang-format` at the repository root.
- Format only files you touched (e.g. `clang-format -i path/to/file.cpp`). Do not reformat unrelated files.
- If `clang-format` is not installed, say so explicitly instead of skipping silently.

---

## When in doubt

- Ask before inventing. Don't guess function signatures, file paths, library APIs, or build invocations. If something isn't in the docs or the code, it doesn't exist yet.
- If two docs disagree, surface the conflict instead of guessing which one is right.
- If a rule conflicts with the task, stop. Don't silently work around the rule.

=========== CMakeLists.txt ===========
cmake_minimum_required(VERSION 3.25)

project(FireDemo VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
include(cmake/FireTargets.cmake)

add_subdirectory(src/sim)
add_subdirectory(src/render)
add_subdirectory(src/metrics)
add_subdirectory(src/engine)
add_subdirectory(src/app)
=========== src/main.cpp ===========
#include "app/MainWindow.hpp"

#include <QApplication>
#include <QtGlobal>

int main(int argc, char* argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication application(argc, argv);
    QApplication::setApplicationDisplayName(QStringLiteral("Classic Fire"));
    QApplication::setOrganizationName(QStringLiteral("Fire Demo"));

    MainWindow window;
    window.show();
    return application.exec();
}
````

</details>

**🔧 Tool call — `Bash`**

*Read sim layer*

```bash
cd src && for f in sim/HeatFrame.hpp sim/FireParameters.hpp sim/FireParameters.cpp sim/FireSimulation.hpp sim/FireSimulation.cpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

```
=========== sim/HeatFrame.hpp ===========
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

class HeatFrame final {
public:
    constexpr HeatFrame(const std::span<const std::uint8_t> cells,
                        const std::size_t width,
                        const std::size_t height) noexcept
        : frameCells(cells), frameWidth(width), frameHeight(height) {
        assert(frameWidth == 0 || frameHeight <= std::numeric_limits<std::size_t>::max() / frameWidth);
        assert(frameCells.size() == frameWidth * frameHeight);
    }

    [[nodiscard]] constexpr std::size_t width() const noexcept { return frameWidth; }
    [[nodiscard]] constexpr std::size_t height() const noexcept { return frameHeight; }
    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept { return frameCells; }

    [[nodiscard]] constexpr std::span<const std::uint8_t> row(const std::size_t y) const noexcept {
        assert(y < frameHeight);
        return frameCells.subspan(y * frameWidth, frameWidth);
    }

private:
    std::span<const std::uint8_t> frameCells;
    std::size_t frameWidth;
    std::size_t frameHeight;
};
=========== sim/FireParameters.hpp ===========
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
    std::uint8_t sourceHeatLevel{MAXIMUM_SOURCE_HEAT};
    std::uint8_t coolingRate{2};
};
=========== sim/FireParameters.cpp ===========
#include "sim/FireParameters.hpp"

#include <algorithm>

void FireParameters::setSourceHeat(const std::uint8_t sourceHeat) noexcept {
    sourceHeatLevel = std::clamp(sourceHeat, MINIMUM_SOURCE_HEAT, MAXIMUM_SOURCE_HEAT);
}

void FireParameters::setCooling(const std::uint8_t cooling) noexcept {
    coolingRate = std::clamp(cooling, MINIMUM_COOLING, MAXIMUM_COOLING);
}
=========== sim/FireSimulation.hpp ===========
#pragma once

#include "sim/FireParameters.hpp"
#include "sim/HeatFrame.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class FireSimulation final {
public:
    explicit FireSimulation(std::size_t width, std::size_t height, std::uint32_t randomSeed = 0xC001CAFEu);

    void tick() noexcept;
    void reset() noexcept;

    [[nodiscard]] std::size_t width() const noexcept { return simulationWidth; }
    [[nodiscard]] std::size_t height() const noexcept { return simulationHeight; }
    [[nodiscard]] HeatFrame heat() const noexcept { return {heatMap, simulationWidth, simulationHeight}; }

    // Parameters clamp themselves, so exposing them mutably cannot invalidate the
    // simulation: tick() re-reads them each frame and holds no derived state.
    [[nodiscard]] FireParameters& parameters() noexcept { return simulationParameters; }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulationParameters; }

private:
    [[nodiscard]] std::uint32_t nextRandom() noexcept;
    void updateFuelRow() noexcept;

    std::size_t simulationWidth;
    std::size_t simulationHeight;
    std::vector<std::uint8_t> heatMap;
    std::uint32_t initialSeed;
    std::uint32_t randomState;
    FireParameters simulationParameters;
};
=========== sim/FireSimulation.cpp ===========
#include "sim/FireSimulation.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace {
constexpr std::uint32_t FALLBACK_SEED = 0xA341316Cu;
constexpr std::uint32_t SIDE_WEIGHT = 1;
constexpr std::uint32_t NEAR_CENTER_WEIGHT = 10;
constexpr std::uint32_t FAR_CENTER_WEIGHT = 4;
constexpr std::uint32_t WEIGHT_DIVISOR = 16;

static_assert(2 * SIDE_WEIGHT + NEAR_CENTER_WEIGHT + FAR_CENTER_WEIGHT == WEIGHT_DIVISOR);
} // namespace

FireSimulation::FireSimulation(const std::size_t width, const std::size_t height, const std::uint32_t randomSeed)
    : simulationWidth(width), simulationHeight(height), initialSeed(randomSeed == 0 ? FALLBACK_SEED : randomSeed),
      randomState(initialSeed) {
    if (simulationWidth < 2 || simulationHeight < 2) {
        throw std::invalid_argument("Fire dimensions must both be at least 2");
    }
    if (simulationWidth > static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) ||
        simulationHeight > std::numeric_limits<std::size_t>::max() / simulationWidth) {
        throw std::length_error("Fire dimensions are too large");
    }

    heatMap.resize(simulationWidth * simulationHeight);
    reset();
}

void FireSimulation::tick() noexcept {
    auto* const cells = heatMap.data();
    const std::uint8_t cooling = simulationParameters.cooling();

    // Propagate from lower rows into the row above. Updating top-to-bottom is
    // intentional: every source row still contains the previous frame's heat.
    for (std::size_t y = 0; y + 1 < simulationHeight; ++y) {
        const std::size_t destinationOffset = y * simulationWidth;
        const std::size_t nearSourceOffset = destinationOffset + simulationWidth;
        const std::size_t farSourceY = std::min(y + 2, simulationHeight - 1);
        const std::size_t farSourceOffset = farSourceY * simulationWidth;

        for (std::size_t x = 0; x < simulationWidth; ++x) {
            const std::size_t leftX = x == 0 ? x : x - 1;
            const std::size_t rightX = x + 1 == simulationWidth ? x : x + 1;
            const std::uint32_t weightedHeat =
                SIDE_WEIGHT * cells[nearSourceOffset + leftX] + NEAR_CENTER_WEIGHT * cells[nearSourceOffset + x] +
                SIDE_WEIGHT * cells[nearSourceOffset + rightX] + FAR_CENTER_WEIGHT * cells[farSourceOffset + x];
            const auto averagedHeat = static_cast<std::uint8_t>(weightedHeat / WEIGHT_DIVISOR);

            cells[destinationOffset + x] =
                averagedHeat > cooling ? static_cast<std::uint8_t>(averagedHeat - cooling) : std::uint8_t{0};
        }
    }

    updateFuelRow();
}

void FireSimulation::reset() noexcept {
    std::fill(heatMap.begin(), heatMap.end(), std::uint8_t{0});
    randomState = initialSeed;
    updateFuelRow();
}

std::uint32_t FireSimulation::nextRandom() noexcept {
    // Xorshift32 is compact, deterministic and adequate for visual noise.
    randomState ^= randomState << 13u;
    randomState ^= randomState >> 17u;
    randomState ^= randomState << 5u;
    return randomState;
}

void FireSimulation::updateFuelRow() noexcept {
    const std::size_t rowOffset = (simulationHeight - 1) * simulationWidth;
    const std::uint8_t sourceHeat = simulationParameters.sourceHeat();
    for (std::size_t x = 0; x < simulationWidth; ++x) {
        const auto flicker = static_cast<std::uint8_t>(nextRandom() & 0x3Fu);
        const auto scaledFlicker = static_cast<std::uint16_t>(flicker) * sourceHeat / 255u;
        heatMap[rowOffset + x] = static_cast<std::uint8_t>(sourceHeat - scaledFlicker);
    }
}
```

</details>

**🔧 Tool call — `Bash`**

*Read render layer*

```bash
cd src && for f in render/PixelBuffer.hpp render/PixelBuffer.cpp render/FirePalette.hpp render/FirePalette.cpp render/FireRenderer.hpp render/FireRenderer.cpp render/Viewport.hpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>⚠️ Result (error)</summary>

```
Exit code 1
(eval):cd:1: no such file or directory: src
```

</details>

**🔧 Tool call — `Bash`**

*Read render layer*

```bash
for f in render/PixelBuffer.hpp render/PixelBuffer.cpp render/FirePalette.hpp render/FirePalette.cpp render/FireRenderer.hpp render/FireRenderer.cpp render/Viewport.hpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

```
=========== render/PixelBuffer.hpp ===========
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

using Rgba32 = std::uint32_t;

class PixelBuffer final {
public:
    void resize(std::size_t width, std::size_t height);

    [[nodiscard]] std::size_t width() const noexcept { return bufferWidth; }
    [[nodiscard]] std::size_t height() const noexcept { return bufferHeight; }
    [[nodiscard]] std::span<Rgba32> row(std::size_t y) noexcept;
    [[nodiscard]] const Rgba32* data() const noexcept { return pixels.data(); }

private:
    std::size_t bufferWidth{0};
    std::size_t bufferHeight{0};
    std::vector<Rgba32> pixels;
};
=========== render/PixelBuffer.cpp ===========
#include "render/PixelBuffer.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

void PixelBuffer::resize(const std::size_t width, const std::size_t height) {
    if (width != 0 && height > std::numeric_limits<std::size_t>::max() / width) {
        throw std::length_error("Pixel buffer dimensions are too large");
    }

    pixels.resize(width * height);
    bufferWidth = width;
    bufferHeight = height;
}

std::span<Rgba32> PixelBuffer::row(const std::size_t y) noexcept {
    assert(y < bufferHeight);
    return std::span<Rgba32>{pixels}.subspan(y * bufferWidth, bufferWidth);
}
=========== render/FirePalette.hpp ===========
#pragma once

#include "render/PixelBuffer.hpp"

#include <array>
#include <cstdint>
#include <span>

struct PaletteStop {
    std::uint8_t index;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

class FirePalette final {
public:
    [[nodiscard]] static FirePalette classic();
    [[nodiscard]] static FirePalette fromStops(std::span<const PaletteStop> stops);

    [[nodiscard]] Rgba32 operator[](std::uint8_t heat) const noexcept;

private:
    explicit FirePalette(std::array<Rgba32, 256> colors) noexcept;

    std::array<Rgba32, 256> colors;
};
=========== render/FirePalette.cpp ===========
#include "render/FirePalette.hpp"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace {
constexpr std::array CLASSIC_PALETTE_STOPS{
    PaletteStop{0, 0, 0, 0},
    PaletteStop{32, 45, 0, 0},
    PaletteStop{80, 150, 0, 0},
    PaletteStop{136, 255, 45, 0},
    PaletteStop{192, 255, 160, 0},
    PaletteStop{232, 255, 245, 80},
    PaletteStop{255, 255, 255, 255},
};

[[nodiscard]] constexpr Rgba32 makeColor(const int red, const int green, const int blue) noexcept {
    return 0xFF000000u | (static_cast<Rgba32>(red) << 16u) | (static_cast<Rgba32>(green) << 8u) |
           static_cast<Rgba32>(blue);
}
} // namespace

FirePalette FirePalette::classic() { return fromStops(CLASSIC_PALETTE_STOPS); }

FirePalette FirePalette::fromStops(const std::span<const PaletteStop> stops) {
    if (stops.size() < 2 || stops.front().index != 0 || stops.back().index != 255) {
        throw std::invalid_argument("A palette must have at least two stops spanning indices 0 through 255");
    }

    std::array<Rgba32, 256> colors{};
    for (std::size_t segment = 0; segment + 1 < stops.size(); ++segment) {
        const PaletteStop& first = stops[segment];
        const PaletteStop& second = stops[segment + 1];
        if (second.index <= first.index) {
            throw std::invalid_argument("Palette stop indices must be strictly increasing");
        }

        const int firstIndex = first.index;
        const int secondIndex = second.index;
        const int distance = secondIndex - firstIndex;
        for (int index = firstIndex; index <= secondIndex; ++index) {
            const int offset = index - firstIndex;
            const int red = first.red + (static_cast<int>(second.red) - first.red) * offset / distance;
            const int green = first.green + (static_cast<int>(second.green) - first.green) * offset / distance;
            const int blue = first.blue + (static_cast<int>(second.blue) - first.blue) * offset / distance;
            colors[static_cast<std::size_t>(index)] = makeColor(red, green, blue);
        }
    }

    return FirePalette{std::move(colors)};
}

Rgba32 FirePalette::operator[](const std::uint8_t heat) const noexcept { return colors[heat]; }

FirePalette::FirePalette(std::array<Rgba32, 256> colors) noexcept : colors(std::move(colors)) {}
=========== render/FireRenderer.hpp ===========
#pragma once

#include "render/FirePalette.hpp"
#include "render/PixelBuffer.hpp"
#include "sim/HeatFrame.hpp"

class FireRenderer final {
public:
    explicit FireRenderer(FirePalette palette) noexcept;

    void setPalette(const FirePalette& palette) noexcept;
    void render(const HeatFrame& heat);

    [[nodiscard]] const PixelBuffer& target() const noexcept { return renderTarget; }

private:
    FirePalette palette;
    PixelBuffer renderTarget;
};
=========== render/FireRenderer.cpp ===========
#include "render/FireRenderer.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

FireRenderer::FireRenderer(FirePalette palette) noexcept : palette(std::move(palette)) {}

void FireRenderer::setPalette(const FirePalette& newPalette) noexcept { palette = newPalette; }

void FireRenderer::render(const HeatFrame& heat) {
    if (renderTarget.width() != heat.width() || renderTarget.height() != heat.height()) {
        renderTarget.resize(heat.width(), heat.height());
    }

    for (std::size_t y = 0; y < heat.height(); ++y) {
        const std::span<const std::uint8_t> source = heat.row(y);
        const std::span<Rgba32> destination = renderTarget.row(y);
        for (std::size_t x = 0; x < source.size(); ++x) {
            destination[x] = palette[source[x]];
        }
    }
}
=========== render/Viewport.hpp ===========
#pragma once

#include <cstdint>

struct FitRect {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
};

[[nodiscard]] constexpr FitRect fitPreservingAspect(const int availableWidth,
                                                    const int availableHeight,
                                                    const int sourceWidth,
                                                    const int sourceHeight) noexcept {
    if (availableWidth <= 0 || availableHeight <= 0 || sourceWidth <= 0 || sourceHeight <= 0) {
        return {};
    }

    const auto widthLimitedHeight = static_cast<std::int64_t>(availableWidth) * sourceHeight;
    const auto heightLimitedWidth = static_cast<std::int64_t>(availableHeight) * sourceWidth;

    int fittedWidth = availableWidth;
    int fittedHeight = availableHeight;
    if (widthLimitedHeight <= heightLimitedWidth) {
        fittedHeight = static_cast<int>(widthLimitedHeight / sourceWidth);
    } else {
        fittedWidth = static_cast<int>(heightLimitedWidth / sourceHeight);
    }

    return {(availableWidth - fittedWidth) / 2, (availableHeight - fittedHeight) / 2, fittedWidth, fittedHeight};
}
```

</details>

**🔧 Tool call — `Bash`**

*Read engine layer*

```bash
for f in engine/FireEngine.hpp engine/FireEngine.cpp engine/FrameClock.hpp engine/FrameClock.cpp engine/FrameReport.hpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

```
=========== engine/FireEngine.hpp ===========
#pragma once

#include "engine/FrameClock.hpp"
#include "engine/FrameReport.hpp"
#include "render/FireRenderer.hpp"
#include "sim/FireParameters.hpp"
#include "sim/FireSimulation.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>

class FireEngine final {
public:
    FireEngine(std::size_t simulationWidth = SIMULATION_WIDTH, std::size_t simulationHeight = SIMULATION_HEIGHT);

    [[nodiscard]] FrameReport advance(std::chrono::steady_clock::duration elapsed);
    void reset() noexcept;

    [[nodiscard]] const PixelBuffer& frame() const noexcept { return renderer.target(); }

    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }
    void setParameters(const FireParameters& parameters) noexcept;

    void setStageTimingEnabled(bool enabled) noexcept { stageTimingEnabled = enabled; }

private:
    static constexpr int SIMULATION_WIDTH = 800;
    static constexpr int SIMULATION_HEIGHT = 600;
    static constexpr int SIMULATION_TICKS_PER_SECOND = 60;
    static constexpr int MAXIMUM_TICKS_PER_WAKE = 3;

    void simulate(int ticks) noexcept;
    void shade();

    FireSimulation simulation;
    FireRenderer renderer;
    FrameClock clock{SIMULATION_TICKS_PER_SECOND, MAXIMUM_TICKS_PER_WAKE};
    std::uint64_t frameIndex{0};
    bool stageTimingEnabled{false};
};
=========== engine/FireEngine.cpp ===========
#include "engine/FireEngine.hpp"

#include "render/FirePalette.hpp"

FireEngine::FireEngine(const std::size_t simulationWidth, const std::size_t simulationHeight)
    : simulation(simulationWidth, simulationHeight), renderer(FirePalette::classic()) {
    renderer.render(simulation.heat());
}

FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) {
    const TickPlan plan = clock.consume(elapsed);
    if (plan.ticks == 0) {
        return FrameReport{0, elapsed, plan.discardedTime, frameIndex, std::nullopt};
    }

    std::optional<FrameStageTimings> stageTimings;
    if (stageTimingEnabled) {
        const auto simulateStartedAt = std::chrono::steady_clock::now();
        simulate(plan.ticks);
        const auto simulateDuration = std::chrono::steady_clock::now() - simulateStartedAt;

        const auto shadeStartedAt = std::chrono::steady_clock::now();
        shade();
        const auto shadeDuration = std::chrono::steady_clock::now() - shadeStartedAt;
        stageTimings = FrameStageTimings{simulateDuration, shadeDuration};
    } else {
        simulate(plan.ticks);
        shade();
    }

    return FrameReport{plan.ticks, elapsed, plan.discardedTime, ++frameIndex, stageTimings};
}

void FireEngine::reset() noexcept {
    simulation.reset();
    renderer.render(simulation.heat());
    clock.reset();
    frameIndex = 0;
}

void FireEngine::setParameters(const FireParameters& parameters) noexcept { simulation.parameters() = parameters; }

void FireEngine::simulate(const int ticks) noexcept {
    for (int tick = 0; tick < ticks; ++tick) {
        simulation.tick();
    }
}

void FireEngine::shade() { renderer.render(simulation.heat()); }
=========== engine/FrameClock.hpp ===========
#pragma once

#include <chrono>

struct TickPlan {
    int ticks{0};
    std::chrono::steady_clock::duration discardedTime{};
};

class FrameClock final {
public:
    FrameClock(int ticksPerSecond, int maximumTicksPerWake);

    [[nodiscard]] TickPlan consume(std::chrono::steady_clock::duration elapsed) noexcept;
    void reset() noexcept;

private:
    using AccumulatorDuration = std::chrono::duration<double>;

    AccumulatorDuration tickDuration;
    int maximumTicksPerWake;
    AccumulatorDuration accumulatedTime{0.0};
};
=========== engine/FrameClock.cpp ===========
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
=========== engine/FrameReport.hpp ===========
#pragma once

#include <chrono>
#include <cstdint>
#include <optional>

struct FrameStageTimings {
    std::chrono::steady_clock::duration simulateDuration{};
    std::chrono::steady_clock::duration shadeDuration{};
};

struct FrameReport {
    int ticksExecuted{0};
    std::chrono::steady_clock::duration elapsed{};
    std::chrono::steady_clock::duration discardedTime{};
    std::uint64_t frameIndex{0};
    std::optional<FrameStageTimings> stageTimings;
};
```

</details>

**🔧 Tool call — `Bash`**

*Read app layer part 1*

```bash
for f in app/MainWindow.hpp app/MainWindow.cpp app/FireController.hpp app/FireController.cpp app/FireView.hpp app/FireView.cpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

```
=========== app/MainWindow.hpp ===========
#pragma once

#include <QMainWindow>

class FireController;
class QEvent;
class StatsPanel;

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void changeEvent(QEvent* event) override;

private:
    FireController* fireController{nullptr};
    StatsPanel* statsPanel{nullptr};
};
=========== app/MainWindow.cpp ===========
#include "app/MainWindow.hpp"

#include "app/ControlPanel.hpp"
#include "app/FireController.hpp"
#include "app/FireView.hpp"
#include "app/FrameMetricsCollector.hpp"
#include "app/StatsPanel.hpp"

#include <QEvent>
#include <QKeySequence>
#include <QShortcut>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Fire Demo"));

    auto* const centralWidget = new QWidget(this);
    auto* const windowLayout = new QVBoxLayout(centralWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    auto* const frameMetricsCollector = new FrameMetricsCollector(this);
    fireController = new FireController(this);

    auto* const renderArea = new QWidget(centralWidget);
    auto* const renderStack = new QStackedLayout(renderArea);
    renderStack->setContentsMargins(0, 0, 0, 0);
    renderStack->setStackingMode(QStackedLayout::StackAll);

    auto* const fireView = new FireView(renderArea);
    fireView->present(fireController->frame());
    renderStack->addWidget(fireView);

    auto* const overlayLayer = new QWidget(renderArea);
    overlayLayer->setAttribute(Qt::WA_NoSystemBackground);
    overlayLayer->setAttribute(Qt::WA_TransparentForMouseEvents);
    auto* const overlayLayout = new QVBoxLayout(overlayLayer);
    overlayLayout->setContentsMargins(12, 12, 12, 12);
    overlayLayout->setSpacing(0);

    statsPanel = new StatsPanel(*frameMetricsCollector, overlayLayer);
    overlayLayout->addWidget(statsPanel, 0, Qt::AlignLeft | Qt::AlignTop);
    overlayLayout->addStretch(1);
    renderStack->addWidget(overlayLayer);
    renderStack->setCurrentWidget(overlayLayer);

    windowLayout->addWidget(renderArea, 1);

    auto* const controlPanel = new ControlPanel(fireController->parameters(), centralWidget);
    windowLayout->addWidget(controlPanel);
    setCentralWidget(centralWidget);

    connect(controlPanel, &ControlPanel::toggleRequested, fireController, &FireController::toggleRunning);
    connect(
        controlPanel, &ControlPanel::metricsEnabledChanged, frameMetricsCollector, &FrameMetricsCollector::setEnabled);
    connect(controlPanel, &ControlPanel::resetRequested, fireController, &FireController::reset);
    connect(controlPanel, &ControlPanel::parametersChanged, fireController, &FireController::setParameters);
    connect(fireController, &FireController::parametersChanged, controlPanel, &ControlPanel::setParameters);
    connect(fireController, &FireController::frameReady, fireView, [this, fireView](const FrameReport&) {
        fireView->present(fireController->frame());
    });
    connect(fireController, &FireController::runningChanged, controlPanel, [controlPanel](const bool running) {
        controlPanel->setPaused(!running);
    });

    connect(frameMetricsCollector,
            &FrameMetricsCollector::enabledChanged,
            fireController,
            &FireController::setMetricsEnabled);
    connect(frameMetricsCollector, &FrameMetricsCollector::enabledChanged, fireView, &FireView::setMetricsEnabled);
    connect(
        frameMetricsCollector, &FrameMetricsCollector::enabledChanged, controlPanel, &ControlPanel::setMetricsEnabled);
    connect(fireController, &FireController::wakeMeasured, frameMetricsCollector, &FrameMetricsCollector::observeWake);
    connect(
        fireController, &FireController::frameMeasured, frameMetricsCollector, &FrameMetricsCollector::observeFrame);
    connect(fireView, &FireView::paintMeasured, frameMetricsCollector, &FrameMetricsCollector::observePaint);

    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    connect(pauseShortcut, &QShortcut::activated, fireController, &FireController::toggleRunning);
    connect(resetShortcut, &QShortcut::activated, fireController, &FireController::reset);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    frameMetricsCollector->setEnabled(true);
    fireController->run();
    resize(960, 720);
}

void MainWindow::changeEvent(QEvent* const event) {
    QMainWindow::changeEvent(event);
    if (event->type() != QEvent::WindowStateChange) {
        return;
    }

    const bool suspended = isMinimized();
    fireController->setSuspended(suspended);
    statsPanel->setSuspended(suspended);
}
=========== app/FireController.hpp ===========
#pragma once

#include "engine/FireEngine.hpp"

#include <QObject>
#include <QTimer>

#include <chrono>

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    [[nodiscard]] bool isRunRequested() const noexcept { return runRequested; }
    [[nodiscard]] bool isSuspended() const noexcept { return suspensionActive; }
    [[nodiscard]] const PixelBuffer& frame() const noexcept { return engine.frame(); }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return engine.parameters(); }

public slots:
    void run();
    void pause();
    void toggleRunning();
    void reset();
    void setParameters(const FireParameters& parameters);
    void setMetricsEnabled(bool enabled) noexcept;
    void setSuspended(bool suspended);

signals:
    void frameReady(FrameReport report);
    void frameMeasured(FrameReport report);
    void wakeMeasured(std::chrono::steady_clock::time_point now);
    void parametersChanged(FireParameters parameters);
    void runningChanged(bool running);

private slots:
    void onWake();

private:
    using Clock = std::chrono::steady_clock;

    static constexpr int WAKE_INTERVAL_MILLISECONDS = 16;

    void updateWakeTimer();

    FireEngine engine;
    QTimer wakeTimer;
    Clock::time_point lastWake;
    bool metricsEnabled{false};
    bool runRequested{false};
    bool suspensionActive{false};
};
=========== app/FireController.cpp ===========
#include "app/FireController.hpp"

FireController::FireController(QObject* const parent) : QObject(parent) {
    wakeTimer.setTimerType(Qt::PreciseTimer);
    wakeTimer.setInterval(WAKE_INTERVAL_MILLISECONDS);
    connect(&wakeTimer, &QTimer::timeout, this, &FireController::onWake);
}

void FireController::run() {
    if (isRunRequested()) {
        return;
    }

    runRequested = true;
    updateWakeTimer();
    emit runningChanged(true);
}

void FireController::pause() {
    if (!isRunRequested()) {
        return;
    }

    runRequested = false;
    updateWakeTimer();
    emit runningChanged(false);
}

void FireController::toggleRunning() {
    if (isRunRequested()) {
        pause();
    } else {
        run();
    }
}

void FireController::reset() {
    engine.reset();
    if (isRunning()) {
        lastWake = Clock::now();
    }

    emit parametersChanged(engine.parameters());
    emit frameReady(FrameReport{});
}

void FireController::setParameters(const FireParameters& parameters) {
    engine.setParameters(parameters);
    emit parametersChanged(engine.parameters());
}

void FireController::setMetricsEnabled(const bool enabled) noexcept {
    metricsEnabled = enabled;
    engine.setStageTimingEnabled(enabled);
}

void FireController::setSuspended(const bool suspended) {
    if (suspensionActive == suspended) {
        return;
    }

    suspensionActive = suspended;
    updateWakeTimer();
}

void 
```

*[truncated, 3767 more characters]*

</details>

**🔧 Tool call — `Bash`**

*Read app layer part 2*

```bash
for f in app/ControlPanel.hpp app/ControlPanel.cpp app/StatsPanel.hpp app/StatsPanel.cpp app/FrameMetricsCollector.hpp app/FrameMetricsCollector.cpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

```
=========== app/ControlPanel.hpp ===========
#pragma once

#include "sim/FireParameters.hpp"

#include <QWidget>

class QPushButton;
class QSlider;

class ControlPanel final : public QWidget {
    Q_OBJECT

public:
    explicit ControlPanel(const FireParameters& parameters, QWidget* parent = nullptr);

    void setPaused(bool paused);
    void setMetricsEnabled(bool enabled);

public slots:
    void setParameters(FireParameters parameters);

signals:
    void toggleRequested();
    void metricsEnabledChanged(bool enabled);
    void resetRequested();
    void parametersChanged(FireParameters parameters);

private:
    [[nodiscard]] FireParameters parametersFromControls() const noexcept;

    QPushButton* pauseButton{nullptr};
    QPushButton* metricsButton{nullptr};
    QSlider* sourceHeatSlider{nullptr};
    QSlider* coolingSlider{nullptr};
};
=========== app/ControlPanel.cpp ===========
#include "app/ControlPanel.hpp"

#include "sim/FireParameters.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSlider>

namespace {
QLabel* makeValueLabel(QWidget* const parent, const int initialValue) {
    auto* const label = new QLabel(QString::number(initialValue), parent);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMinimumWidth(28);
    return label;
}

QSlider* addSlider(QHBoxLayout& layout,
                   QWidget* const parent,
                   const QString& title,
                   const int minimum,
                   const int maximum,
                   const int initialValue) {
    auto* const titleLabel = new QLabel(title, parent);
    auto* const slider = new QSlider(Qt::Horizontal, parent);
    auto* const valueLabel = makeValueLabel(parent, initialValue);
    slider->setRange(minimum, maximum);
    slider->setValue(initialValue);
    slider->setMinimumWidth(90);

    QObject::connect(
        slider, &QSlider::valueChanged, valueLabel, [valueLabel](const int value) { valueLabel->setNum(value); });

    layout.addWidget(titleLabel);
    layout.addWidget(slider, 1);
    layout.addWidget(valueLabel);
    return slider;
}
} // namespace

ControlPanel::ControlPanel(const FireParameters& parameters, QWidget* const parent) : QWidget(parent) {
    auto* const controlsLayout = new QHBoxLayout(this);
    controlsLayout->setContentsMargins(12, 8, 12, 8);

    pauseButton = new QPushButton(QStringLiteral("Pause"), this);
    metricsButton = new QPushButton(QStringLiteral("Metrics"), this);
    auto* const resetButton = new QPushButton(QStringLiteral("Reset"), this);
    metricsButton->setObjectName(QStringLiteral("metricsButton"));
    metricsButton->setCheckable(true);
    pauseButton->setToolTip(QStringLiteral("Pause or resume (Space)"));
    metricsButton->setToolTip(QStringLiteral("Show or hide performance metrics"));
    resetButton->setToolTip(QStringLiteral("Restart the simulation (R)"));
    controlsLayout->addWidget(metricsButton);
    controlsLayout->addWidget(pauseButton);
    controlsLayout->addWidget(resetButton);
    controlsLayout->addSpacing(8);

    sourceHeatSlider = addSlider(*controlsLayout,
                                 this,
                                 QStringLiteral("Heat"),
                                 FireParameters::MINIMUM_SOURCE_HEAT,
                                 FireParameters::MAXIMUM_SOURCE_HEAT,
                                 parameters.sourceHeat());
    controlsLayout->addSpacing(8);
    coolingSlider = addSlider(*controlsLayout,
                              this,
                              QStringLiteral("Cooling"),
                              FireParameters::MINIMUM_COOLING,
                              FireParameters::MAXIMUM_COOLING,
                              parameters.cooling());

    connect(pauseButton, &QPushButton::clicked, this, &ControlPanel::toggleRequested);
    connect(metricsButton, &QPushButton::toggled, this, &ControlPanel::metricsEnabledChanged);
    connect(resetButton, &QPushButton::clicked, this, &ControlPanel::resetRequested);
    connect(
        sourceHeatSlider, &QSlider::valueChanged, this, [this] { emit parametersChanged(parametersFromControls()); });
    connect(coolingSlider, &QSlider::valueChanged, this, [this] { emit parametersChanged(parametersFromControls()); });
}

void ControlPanel::setPaused(const bool paused) {
    pauseButton->setText(paused ? QStringLiteral("Resume") : QStringLiteral("Pause"));
}

void ControlPanel::setMetricsEnabled(const bool enabled) {
    const QSignalBlocker signalBlocker{metricsButton};
    metricsButton->setChecked(enabled);
}

void ControlPanel::setParameters(FireParameters parameters) {
    const QSignalBlocker signalBlocker{this};
    sourceHeatSlider->setValue(parameters.sourceHeat());
    coolingSlider->setValue(parameters.cooling());
}

FireParameters ControlPanel::parametersFromControls() const noexcept {
    FireParameters parameters;
    parameters.setSourceHeat(static_cast<std::uint8_t>(sourceHeatSlider->value()));
    parameters.setCooling(static_cast<std::uint8_t>(coolingSlider->value()));
    return parameters;
}
=========== app/StatsPanel.hpp ===========
#pragma once

#include <QWidget>

class FrameMetricsCollector;
class QLabel;
class QPaintEvent;
class QTimer;

class StatsPanel final : public QWidget {
public:
    explicit StatsPanel(const FrameMetricsCollector& metricsCollector, QWidget* parent = nullptr);
    void setSuspended(bool suspended);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static constexpr int REFRESH_INTERVAL_MILLISECONDS = 250;

    void refresh();
    void updateRefreshTimer();

    const FrameMetricsCollector& metricsCollector;
    QLabel* windowSummaryLabel{nullptr};
    QLabel* metricRowsLabel{nullptr};
    QLabel* latestFrameLabel{nullptr};
    QTimer* refreshTimer{nullptr};
    bool suspensionActive{false};
};
=========== app/StatsPanel.cpp ===========
#include "app/StatsPanel.hpp"

#include "app/FrameMetricsCollector.hpp"

#include <QColor>
#include <QFontDatabase>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QSizePolicy>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>

#include <chrono>
#include <cstddef>

namespace {
QString formatMetricValue(const double value, const std::size_t sampleCount) {
    return sampleCount == 0 ? QStringLiteral("--") : QString::number(value, 'f', 2);
}

QString formatMetricRow(const QString& name, const MetricStatistics& statistics) {
    return QStringLiteral("%1 %2 %3 %4")
        .arg(name, -15)
        .arg(formatMetricValue(statistics.averageMilliseconds, statistics.sampleCount), 8)
        .arg(formatMetricValue(statistics.percentile95Milliseconds, statistics.sampleCount), 8)
        .arg(formatMetricValue(statistics.maximumMilliseconds, statistics.sampleCount), 8);
}

QString formatWindowSummary(const MetricStatistics& wakeInterval) {
    const double windowSeconds =
        wakeInterval.averageMilliseconds * static_cast<double>(wakeInterval.sampleCount) / 1000.0;
    return QStringLiteral("Window %1 s | %2 samples")
        .arg(windowSeconds, 0, 'f', 2)
        .arg(static_cast<qulonglong>(wakeInterval.sampleCount));
}
} // namespace

StatsPanel::StatsPanel(const FrameMetricsCollector& metricsCollector, QWidget* const parent)
    : QWidget(parent), metricsCollector(metricsCollector) {
    setObjectName(QStringLiteral("statsPanel"));
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QPalette panelPalette = palette();
    panelPalette.setColor(QPalette::Window, QColor{24, 24, 24});
    panelPalette.setColor(QPalette::WindowText, QColor{240, 240, 240});
    setPalette(panelPalette);

    auto* const panelLayout = new QVBoxLayout(this);
    panelLayout->setContentsMargins(12, 10, 12, 10);
    panelLayout->setSpacing(4);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDa
```

*[truncated, 6582 more characters]*

</details>

**🔧 Tool call — `Bash`**

*Read metrics layer*

```bash
for f in metrics/MetricsClock.hpp metrics/MetricStatistics.hpp metrics/IntervalMetric.hpp metrics/IntervalMetric.cpp metrics/TimeSeriesMetric.hpp metrics/TimeSeriesMetric.cpp; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>Result</summary>

```
=========== metrics/MetricsClock.hpp ===========
#pragma once

#include <chrono>

using MetricsClock = std::chrono::steady_clock;
=========== metrics/MetricStatistics.hpp ===========
#pragma once

#include <cstddef>

struct MetricStatistics {
    double averageMilliseconds{0.0};
    double percentile95Milliseconds{0.0};
    double maximumMilliseconds{0.0};
    std::size_t sampleCount{0};
};
=========== metrics/IntervalMetric.hpp ===========
#pragma once

#include "metrics/MetricsClock.hpp"
#include "metrics/TimeSeriesMetric.hpp"

class IntervalMetric final {
public:
    void mark(MetricsClock::time_point now) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    TimeSeriesMetric intervals;
    MetricsClock::time_point previousMark{};
    bool hasPreviousMark{false};
};
=========== metrics/IntervalMetric.cpp ===========
#include "metrics/IntervalMetric.hpp"

void IntervalMetric::mark(const MetricsClock::time_point now) noexcept {
    if (hasPreviousMark) {
        intervals.record(now - previousMark);
    }

    previousMark = now;
    hasPreviousMark = true;
}

void IntervalMetric::clear() noexcept {
    intervals.clear();
    previousMark = {};
    hasPreviousMark = false;
}

MetricStatistics IntervalMetric::statistics() const noexcept { return intervals.statistics(); }
=========== metrics/TimeSeriesMetric.hpp ===========
#pragma once

#include "metrics/MetricStatistics.hpp"
#include "metrics/MetricsClock.hpp"

#include <array>
#include <cstddef>

class TimeSeriesMetric final {
public:
    void record(MetricsClock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;

private:
    static constexpr std::size_t MAXIMUM_SAMPLE_COUNT = 512;

    std::array<MetricsClock::duration, MAXIMUM_SAMPLE_COUNT> samples{};
    std::size_t nextSample{0};
    std::size_t sampleCount{0};
};
=========== metrics/TimeSeriesMetric.cpp ===========
#include "metrics/TimeSeriesMetric.hpp"

#include <algorithm>

namespace {
double toMilliseconds(const MetricsClock::duration duration) noexcept {
    return std::chrono::duration<double, std::milli>{duration}.count();
}
} // namespace

void TimeSeriesMetric::record(const MetricsClock::duration duration) noexcept {
    samples[nextSample] = duration;
    nextSample = (nextSample + 1) % samples.size();
    sampleCount = std::min(sampleCount + 1, samples.size());
}

void TimeSeriesMetric::clear() noexcept {
    nextSample = 0;
    sampleCount = 0;
}

MetricStatistics TimeSeriesMetric::statistics() const noexcept {
    if (sampleCount == 0) {
        return {};
    }

    std::array<MetricsClock::duration, MAXIMUM_SAMPLE_COUNT> sortedSamples{};
    double totalMilliseconds = 0.0;
    for (std::size_t index = 0; index < sampleCount; ++index) {
        sortedSamples[index] = samples[index];
        totalMilliseconds += toMilliseconds(samples[index]);
    }

    const auto samplesEnd = sortedSamples.begin() + static_cast<std::ptrdiff_t>(sampleCount);
    std::sort(sortedSamples.begin(), samplesEnd);

    const std::size_t percentileIndex = (sampleCount * 95 + 99) / 100 - 1;
    return {
        totalMilliseconds / static_cast<double>(sampleCount),
        toMilliseconds(sortedSamples[percentileIndex]),
        toMilliseconds(sortedSamples[sampleCount - 1]),
        sampleCount,
    };
}
```

</details>

**🔧 Tool call — `Bash`**

*Read cmake files*

```bash
for f in cmake/FireTargets.cmake cmake/ProjectSettings.cmake cmake/Sanitizers.cmake src/sim/CMakeLists.txt src/render/CMakeLists.txt src/metrics/CMakeLists.txt src/engine/CMakeLists.txt src/app/CMakeLists.txt; do echo "=========== $f ==========="; cat "$f"; done
```

<details>
<summary>⚠️ Result (error)</summary>

```
Exit code 1
=========== cmake/FireTargets.cmake ===========
cat: cmake/FireTargets.cmake: No such file or directory
=========== cmake/ProjectSettings.cmake ===========
cat: cmake/ProjectSettings.cmake: No such file or directory
=========== cmake/Sanitizers.cmake ===========
cat: cmake/Sanitizers.cmake: No such file or directory
=========== src/sim/CMakeLists.txt ===========
cat: src/sim/CMakeLists.txt: No such file or directory
=========== src/render/CMakeLists.txt ===========
cat: src/render/CMakeLists.txt: No such file or directory
=========== src/metrics/CMakeLists.txt ===========
cat: src/metrics/CMakeLists.txt: No such file or directory
=========== src/engine/CMakeLists.txt ===========
cat: src/engine/CMakeLists.txt: No such file or directory
=========== src/app/CMakeLists.txt ===========
cat: src/app/CMakeLists.txt: No such file or directory
```

</details>

**🔧 Tool call — `Bash`**

*Read cmake files with absolute path*

```bash
cd /Users/User/private/fire && for f in cmake/FireTargets.cmake cmake/ProjectSettings.cmake src/sim/CMakeLists.txt src/app/CMakeLists.txt src/engine/CMakeLists.txt; do echo "=========== $f ==========="; cat "$f"; done; ls -a
```

<details>
<summary>Result</summary>

```
=========== cmake/FireTargets.cmake ===========
include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/ProjectSettings.cmake")

function(fire_add_library targetName)
    set(multiValueArgs SOURCES PUBLIC_DEPENDENCIES PRIVATE_DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 1 fireLibrary "" "" "${multiValueArgs}")

    if(fireLibrary_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unsupported arguments for ${targetName}: ${fireLibrary_UNPARSED_ARGUMENTS}"
        )
    endif()

    if(NOT fireLibrary_SOURCES)
        message(FATAL_ERROR "No sources specified for library ${targetName}")
    endif()

    add_library("${targetName}" ${fireLibrary_SOURCES})
    target_include_directories("${targetName}" PUBLIC "${PROJECT_SOURCE_DIR}/src")

    if(fireLibrary_PUBLIC_DEPENDENCIES)
        target_link_libraries("${targetName}" PUBLIC ${fireLibrary_PUBLIC_DEPENDENCIES})
    endif()
    if(fireLibrary_PRIVATE_DEPENDENCIES)
        target_link_libraries("${targetName}" PRIVATE ${fireLibrary_PRIVATE_DEPENDENCIES})
    endif()

    fire_apply_project_settings("${targetName}" PUBLIC)
endfunction()

function(fire_add_executable targetName)
    set(options AUTOMOC INCLUDE_SOURCE_ROOT)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 1 fireExecutable "${options}" "" "${multiValueArgs}")

    if(fireExecutable_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unsupported arguments for ${targetName}: ${fireExecutable_UNPARSED_ARGUMENTS}"
        )
    endif()

    if(NOT fireExecutable_SOURCES)
        message(FATAL_ERROR "No sources specified for executable ${targetName}")
    endif()

    add_executable("${targetName}" ${fireExecutable_SOURCES})

    if(fireExecutable_INCLUDE_SOURCE_ROOT)
        target_include_directories("${targetName}" PRIVATE "${PROJECT_SOURCE_DIR}/src")
    endif()
    if(fireExecutable_DEPENDENCIES)
        target_link_libraries("${targetName}" PRIVATE ${fireExecutable_DEPENDENCIES})
    endif()
    if(fireExecutable_AUTOMOC)
        set_target_properties("${targetName}" PROPERTIES AUTOMOC TRUE)
    endif()

    fire_apply_project_settings("${targetName}" PRIVATE)
endfunction()

function(fire_add_test targetName)
    if(NOT BUILD_TESTING)
        return()
    endif()

    set(options AUTOMOC INCLUDE_SOURCE_ROOT)
    set(multiValueArgs SOURCES DEPENDENCIES ENVIRONMENT)
    cmake_parse_arguments(PARSE_ARGV 1 fireTest "${options}" "" "${multiValueArgs}")

    if(fireTest_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unsupported arguments for ${targetName}: ${fireTest_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(executableArguments
        SOURCES ${fireTest_SOURCES}
        DEPENDENCIES ${fireTest_DEPENDENCIES}
    )
    if(fireTest_AUTOMOC)
        list(APPEND executableArguments AUTOMOC)
    endif()
    if(fireTest_INCLUDE_SOURCE_ROOT)
        list(APPEND executableArguments INCLUDE_SOURCE_ROOT)
    endif()

    fire_add_executable("${targetName}" ${executableArguments})
    target_sources("${targetName}" PRIVATE "${PROJECT_SOURCE_DIR}/tests/tests_common.h")
    target_include_directories("${targetName}" PRIVATE "${PROJECT_SOURCE_DIR}/tests")
    add_test(NAME "${targetName}" COMMAND "${targetName}")
    set_tests_properties("${targetName}" PROPERTIES WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

    if(fireTest_ENVIRONMENT)
        set_tests_properties("${targetName}" PROPERTIES ENVIRONMENT "${fireTest_ENVIRONMENT}")
    endif()
endfunction()
=========== cmake/ProjectSettings.cmake ===========
include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake")

function(fire_apply_project_settings targetName languageScope)
    if(NOT TARGET "${targetName}")
        message(FATAL_ERROR "Cannot configure unknown target: ${targetName}")
    endif()

    if(NOT languageScope MATCHES "^(PRIVATE|PUBLIC|INTERFACE)$")
        message(FATAL_ERROR "Invalid C++ language setting scope: ${languageScope}")
    endif()

    target_compile_features("${targetName}" ${languageScope} cxx_std_20)

    if(MSVC)
        target_compile_options("${targetName}" PRIVATE /W4 /permissive-)
    else()
        target_compile_options("${targetName}" PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    fire_enable_sanitizers("${targetName}")

    set_target_properties("${targetName}" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
endfunction()
=========== src/sim/CMakeLists.txt ===========
fire_add_library(fire_simulation
    SOURCES
        FireParameters.cpp
        FireParameters.hpp
        FireSimulation.cpp
        FireSimulation.hpp
        HeatFrame.hpp
)

fire_add_test(fire_simulation_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/sim/FireSimulationTests.cpp"
    DEPENDENCIES
        fire_simulation
)
=========== src/app/CMakeLists.txt ===========
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(qtWidgetsTarget "Qt${QT_VERSION_MAJOR}::Widgets")

fire_add_executable(fire_demo
    AUTOMOC
    INCLUDE_SOURCE_ROOT
    SOURCES
        ControlPanel.cpp
        ControlPanel.hpp
        FireController.cpp
        FireController.hpp
        FireView.cpp
        FireView.hpp
        FrameMetricsCollector.cpp
        FrameMetricsCollector.hpp
        MainWindow.cpp
        MainWindow.hpp
        StatsPanel.cpp
        StatsPanel.hpp
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
    DEPENDENCIES
        fire_engine
        fire_metrics
        ${qtWidgetsTarget}
)

set_target_properties(fire_demo PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Classic Fire"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.classicfire"
    WIN32_EXECUTABLE TRUE
)

add_custom_target(run_app
    COMMAND "$<TARGET_FILE:fire_demo>"
    DEPENDS fire_demo
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:fire_demo>"
    USES_TERMINAL
    COMMENT "Starting Fire"
)

fire_add_test(fire_app_metrics_tests
    AUTOMOC
    INCLUDE_SOURCE_ROOT
    SOURCES
        FrameMetricsCollector.cpp
        FrameMetricsCollector.hpp
        "${PROJECT_SOURCE_DIR}/tests/app/FrameMetricsCollectorTests.cpp"
    DEPENDENCIES
        fire_engine
        fire_metrics
        ${qtWidgetsTarget}
)

fire_add_test(fire_app_controller_tests
    AUTOMOC
    INCLUDE_SOURCE_ROOT
    SOURCES
        FireController.cpp
        FireController.hpp
        "${PROJECT_SOURCE_DIR}/tests/app/FireControllerTests.cpp"
    DEPENDENCIES
        fire_engine
        ${qtWidgetsTarget}
)

fire_add_test(fire_app_control_panel_tests
    AUTOMOC
    INCLUDE_SOURCE_ROOT
    SOURCES
        ControlPanel.cpp
        ControlPanel.hpp
        "${PROJECT_SOURCE_DIR}/tests/app/ControlPanelTests.cpp"
    DEPENDENCIES
        fire_simulation
        ${qtWidgetsTarget}
    ENVIRONMENT
        QT_QPA_PLATFORM=offscreen
)

fire_add_test(fire_app_stats_panel_tests
    AUTOMOC
    INCLUDE_SOURCE_ROOT
    SOURCES
        FrameMetricsCollector.cpp
        FrameMetricsCollector.hpp
        StatsPanel.cpp
        StatsPanel.hpp
        "${PROJECT_SOURCE_DIR}/tests/app/StatsPanelTests.cpp"
    DEPENDENCIES
        fire_engine
        fire_metrics
        ${qtWidgetsTarget}
    ENVIRONMENT
        QT_QPA_PLATFORM=offscreen
)
=========== src/engine/CMakeLists.txt ===========
fire_add_library(fire_engine
    SOURCES
        FireEngine.cpp
        FireEngine.hpp
        FrameClock.cpp
        FrameClock.hpp
        FrameReport.hpp
    PUBLIC_DEPENDENCIES
        fire_simulation
        fire_render
)

fire_add_test(fire_engine_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/engine/FrameClockTests.cpp"
    DEPENDENCIES
        fire_engine
)
.
..
.DS_Store
.clang-format
.git
.gitignore
AGENTS.md
CMakeLists.txt
CMakePresets.json
README.md
build
cmake
docs
justfile
src
tests
```

</details>

**🔧 Tool call — `Bash`**

*List docs and read test common header*

```bash
ls -R docs && echo "--- tests_common ---" && cat tests/tests_common.h
```

<details>
<summary>Result</summary>

```
--- tests_common ---
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
```

</details>

**🔧 Tool call — `Bash`**

*Grep for symbol usage*

```bash
ls -la docs; echo "--- grep usage ---"; for sym in setPalette fromStops "\.cells()" "isRunRequested" "isSuspended" "isRunning" "->pause()" "\.pause()" "clear()" "discardedTime" "elapsed" "paintInterval" "frameReady" "frameMeasured" "latestFrame" "minimumSizeHint" "sizeHint" "Viewport" "MetricsClock"; do echo "== $sym"; grep -rn "$sym" src tests --include=*.cpp --include=*.hpp --include=*.h | sed 's/^/   /'; done
```

<details>
<summary>Result</summary>

```
total 0
drwxr-xr-x@  2 User  staff   64 Aug 26 15:49 .
drwxr-xr-x@ 16 User  staff  512 Aug 26 16:59 ..
--- grep usage ---
== setPalette
(eval):1: no matches found: --include=*.cpp
== fromStops
(eval):1: no matches found: --include=*.cpp
== \.cells()
(eval):1: no matches found: --include=*.cpp
== isRunRequested
(eval):1: no matches found: --include=*.cpp
== isSuspended
(eval):1: no matches found: --include=*.cpp
== isRunning
(eval):1: no matches found: --include=*.cpp
== ->pause()
(eval):1: no matches found: --include=*.cpp
== \.pause()
(eval):1: no matches found: --include=*.cpp
== clear()
(eval):1: no matches found: --include=*.cpp
== discardedTime
(eval):1: no matches found: --include=*.cpp
== elapsed
(eval):1: no matches found: --include=*.cpp
== paintInterval
(eval):1: no matches found: --include=*.cpp
== frameReady
(eval):1: no matches found: --include=*.cpp
== frameMeasured
(eval):1: no matches found: --include=*.cpp
== latestFrame
(eval):1: no matches found: --include=*.cpp
== minimumSizeHint
(eval):1: no matches found: --include=*.cpp
== sizeHint
(eval):1: no matches found: --include=*.cpp
== Viewport
(eval):1: no matches found: --include=*.cpp
== MetricsClock
(eval):1: no matches found: --include=*.cpp
```

</details>

**🔧 Tool call — `Bash`**

*Grep symbol usage without include filters*

```bash
for sym in setPalette fromStops 'cells()' isRunRequested isSuspended isRunning 'pause()' discardedTime 'report.elapsed' paintInterval latestFrame; do echo "== $sym"; grep -rn -- "$sym" src tests | sed 's/^/   /'; done
```

<details>
<summary>Result</summary>

```
== setPalette
   src/app/StatsPanel.cpp:51:    setPalette(panelPalette);
   src/render/FireRenderer.cpp:10:void FireRenderer::setPalette(const FirePalette& newPalette) noexcept { palette = newPalette; }
   src/render/FireRenderer.hpp:11:    void setPalette(const FirePalette& palette) noexcept;
   tests/render/RenderTests.cpp:134:    renderer.setPalette(FirePalette::fromStops(replacementStops));
== fromStops
   src/render/FirePalette.cpp:25:FirePalette FirePalette::classic() { return fromStops(CLASSIC_PALETTE_STOPS); }
   src/render/FirePalette.cpp:27:FirePalette FirePalette::fromStops(const std::span<const PaletteStop> stops) {
   src/render/FirePalette.hpp:19:    [[nodiscard]] static FirePalette fromStops(std::span<const PaletteStop> stops);
   tests/render/RenderTests.cpp:68:    const FirePalette palette = FirePalette::fromStops(stops);
   tests/render/RenderTests.cpp:93:    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(noStops)); },
   tests/render/RenderTests.cpp:95:    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(missingFirstStop)); },
   tests/render/RenderTests.cpp:97:    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(missingLastStop)); },
   tests/render/RenderTests.cpp:99:    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(duplicateStop)); },
   tests/render/RenderTests.cpp:101:    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(descendingStops)); },
   tests/render/RenderTests.cpp:116:    FireRenderer renderer{FirePalette::fromStops(paletteStops)};
   tests/render/RenderTests.cpp:134:    renderer.setPalette(FirePalette::fromStops(replacementStops));
== cells()
   src/sim/HeatFrame.hpp:21:    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept { return frameCells; }
   tests/sim/FireSimulationTests.cpp:39:    for (const std::uint8_t cell : heat.cells()) {
   tests/sim/FireSimulationTests.cpp:60:    check(heat.cells().size() == SIMULATION_WIDTH * SIMULATION_HEIGHT, "heat frame matches its geometry");
   tests/sim/FireSimulationTests.cpp:62:    check(heat.row(SIMULATION_HEIGHT - 1).data() == heat.cells().data() + (SIMULATION_HEIGHT - 1) * SIMULATION_WIDTH,
== isRunRequested
   src/app/FireController.cpp:10:    if (isRunRequested()) {
   src/app/FireController.cpp:20:    if (!isRunRequested()) {
   src/app/FireController.cpp:30:    if (isRunRequested()) {
   src/app/FireController.hpp:17:    [[nodiscard]] bool isRunRequested() const noexcept { return runRequested; }
   tests/app/FireControllerTests.cpp:19:    check(!controller.isRunRequested(), "the controller starts logically paused");
   tests/app/FireControllerTests.cpp:24:    check(controller.isRunRequested(), "run records the requested running state");
   tests/app/FireControllerTests.cpp:29:    check(controller.isRunRequested(), "suspension preserves the requested running state");
   tests/app/FireControllerTests.cpp:35:    check(controller.isRunRequested(), "restore preserves the requested running state");
   tests/app/FireControllerTests.cpp:48:    check(!controller.isRunRequested(), "restore preserves the requested paused state");
   tests/app/FireControllerTests.cpp:58:    check(controller.isRunRequested(), "run can be requested while suspended");
== isSuspended
   src/app/FireController.hpp:18:    [[nodiscard]] bool isSuspended() const noexcept { return suspensionActive; }
   tests/app/FireControllerTests.cpp:21:    check(!controller.isSuspended(), "the controller starts unsuspended");
   tests/app/FireControllerTests.cpp:31:    check(controller.isSuspended(), "suspension is observable");
   tests/app/FireControllerTests.cpp:50:    check(!controller.isSuspended(), "the controller leaves suspension after restore");
== isRunning
   src/app/FireController.cpp:39:    if (isRunning()) {
   src/app/FireController.cpp:68:    if (shouldAdvance == isRunning()) {
   src/app/FireController.hpp:16:    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
   tests/app/FireControllerTests.cpp:20:    check(!controller.isRunning(), "the controller starts with its wake timer stopped");
   tests/app/FireControllerTests.cpp:25:    check(controller.isRunning(), "run starts frame advancement while unsuspended");
   tests/app/FireControllerTests.cpp:30:    check(!controller.isRunning(), "suspension stops frame advancement");
   tests/app/FireControllerTests.cpp:36:    check(controller.isRunning(), "restore resumes requested frame advancement");
   tests/app/FireControllerTests.cpp:49:    check(!controller.isRunning(), "restore does not advance a user-paused controller");
   tests/app/FireControllerTests.cpp:59:    check(!controller.isRunning(), "a suspended controller defers requested advancement");
   tests/app/FireControllerTests.cpp:62:    check(controller.isRunning(), "restore starts advancement requested during suspension");
== pause()
   src/app/FireController.cpp:19:void FireController::pause() {
   src/app/FireController.cpp:31:        pause();
   src/app/FireController.hpp:24:    void pause();
   tests/app/FireControllerTests.cpp:43:    controller.pause();
== discardedTime
   src/engine/FrameClock.cpp:31:    AccumulatorDuration discardedTime{0.0};
   src/engine/FrameClock.cpp:33:        discardedTime = accumulatedTime - maximumCatchUpTime;
   src/engine/FrameClock.cpp:43:    return TickPlan{ticks, std::chrono::duration_cast<std::chrono::steady_clock::duration>(discardedTime)};
   src/engine/FrameClock.hpp:7:    std::chrono::steady_clock::duration discardedTime{};
   src/engine/FireEngine.cpp:13:        return FrameReport{0, elapsed, plan.discardedTime, frameIndex, std::nullopt};
   src/engine/FireEngine.cpp:31:    return FrameReport{plan.ticks, elapsed, plan.discardedTime, ++frameIndex, stageTimings};
   src/engine/FrameReport.hpp:15:    std::chrono::steady_clock::duration discardedTime{};
   tests/engine/FrameClockTests.cpp:20:    check(plan.ticks == expectedTicks && plan.discardedTime == expectedDiscardedTime, message);
   tests/engine/FrameClockTests.cpp:76:    check(idleReport.discardedTime == 0ns, "an ordinary engine wake discards no time");
   tests/engine/FrameClockTests.cpp:110:    check(report.discardedTime > 0ns, "the engine reports time discarded by its catch-up clamp");
== report.elapsed
== paintInterval
   src/app/FrameMetricsCollector.hpp:19:    MetricStatistics paintInterval;
   src/app/FrameMetricsCollector.hpp:47:    IntervalMetric paintInterval;
   src/app/FrameMetricsCollector.cpp:14:        paintInterval.statistics(),
   src/app/FrameMetricsCollector.cpp:36:    paintInterval.clear();
   src/app/FrameMetricsCollector.cpp:67:    paintInterval.mark(startedAt);
   src/app/StatsPanel.cpp:122:    metricRows.append(formatMetricRow(QStringLiteral("Paint interval"), snapshot.paintInterval));
   tests/app/FrameMetricsCollectorTests.cpp:54:    checkNear(snapshot.paintInterval.averageMilliseconds, 16.0, "collector records paint start intervals");
   tests/app/FrameMetricsCollectorTests.cpp:55:    check(snapshot.paintInterval.sampleCount == 1, "the first paint establishes the interval reference");
   tests/app/FrameMetricsCollectorTests.cpp:75:    checkEmpty(snapshot.paintInterval, "disabled collector ignores paint interval observations");
   tests/app/FrameMetricsCollectorTests.cpp:105:    checkEmpty(reenabledSnapshot.paintInterval, "re-enabling resets paint interval history");
== latestFrame
   src/app/FrameMetricsCollector.hpp:20:    std::optional<FrameReport> latestFrame;
   src/app/FrameMetricsCollector.hpp:49:    std::optional<FrameReport> latestFrame;
   src/app/StatsPanel.hpp:27:    QLabel* latestFrameLabel{nullptr};
   src/app/FrameMetricsCollector.cpp:15:        latestFrame,
   src/app/FrameMetricsCollector.cpp:37:    latestFrame.reset();
   src/app/FrameMetricsCollector.cpp:57:    latestFrame = s
```

*[truncated, 2015 more characters]*

</details>

**🔧 Tool call — `Bash`**

*Grep more symbols*

```bash
for sym in '\.elapsed' 'snapshot.enabled' '\.enabled' 'FireEngine ' 'FireEngine{' 'frameIndex' 'wakeMeasured' 'metricsEnabled' 'MAXIMUM_TICKS_PER_WAKE' 'setStageTimingEnabled'; do echo "== $sym"; grep -rn -- "$sym" src tests | sed 's/^/   /'; done
```

<details>
<summary>Result</summary>

```
== \.elapsed
   tests/engine/FrameClockTests.cpp:75:    check(idleReport.elapsed == 16ms, "an engine report retains its elapsed wall time");
== snapshot.enabled
   tests/app/FrameMetricsCollectorTests.cpp:47:    check(snapshot.enabled, "snapshot reports that collection is enabled");
   tests/app/FrameMetricsCollectorTests.cpp:70:    check(!snapshot.enabled, "collector starts disabled");
== \.enabled
   tests/app/FrameMetricsCollectorTests.cpp:47:    check(snapshot.enabled, "snapshot reports that collection is enabled");
   tests/app/FrameMetricsCollectorTests.cpp:70:    check(!snapshot.enabled, "collector starts disabled");
   tests/app/FrameMetricsCollectorTests.cpp:94:    check(!disabledSnapshot.enabled, "collector reports its disabled state");
   tests/app/FrameMetricsCollectorTests.cpp:102:    check(reenabledSnapshot.enabled, "collector can be re-enabled");
== FireEngine 
   src/app/FireController.hpp:48:    FireEngine engine;
   src/engine/FireEngine.hpp:13:class FireEngine final {
   tests/engine/FrameClockTests.cpp:67:    FireEngine engine{8, 6};
   tests/engine/FrameClockTests.cpp:99:    FireEngine engine{8, 6};
   tests/engine/FrameClockTests.cpp:115:    FireEngine engine{8, 6};
== FireEngine{
== frameIndex
   src/app/StatsPanel.cpp:132:    latestFrameLabel->setText(QStringLiteral("Frame %1").arg(static_cast<qulonglong>(frame.frameIndex)));
   src/engine/FireEngine.hpp:39:    std::uint64_t frameIndex{0};
   src/engine/FireEngine.cpp:13:        return FrameReport{0, elapsed, plan.discardedTime, frameIndex, std::nullopt};
   src/engine/FireEngine.cpp:31:    return FrameReport{plan.ticks, elapsed, plan.discardedTime, ++frameIndex, stageTimings};
   src/engine/FireEngine.cpp:38:    frameIndex = 0;
   src/engine/FrameReport.hpp:16:    std::uint64_t frameIndex{0};
   tests/app/FrameMetricsCollectorTests.cpp:56:    check(snapshot.latestFrame.has_value() && snapshot.latestFrame->frameIndex == 7,
   tests/app/FrameMetricsCollectorTests.cpp:97:    check(disabledSnapshot.latestFrame.has_value() && disabledSnapshot.latestFrame->frameIndex == 1,
   tests/engine/FrameClockTests.cpp:77:    check(idleReport.frameIndex == 0, "a zero-tick wake does not advance the frame index");
   tests/engine/FrameClockTests.cpp:85:    check(frameReport.frameIndex == 1, "a produced frame advances the frame index");
   tests/engine/FrameClockTests.cpp:111:    check(report.frameIndex == 1, "multiple ticks in one wake produce one frame");
   tests/engine/FrameClockTests.cpp:125:    check(idleReport.ticksExecuted == 0 && idleReport.frameIndex == 0,
   tests/engine/FrameClockTests.cpp:129:    check(frameReport.ticksExecuted == 1 && frameReport.frameIndex == 1, "engine produces frames normally after reset");
== wakeMeasured
   src/app/FireController.cpp:86:        emit wakeMeasured(now);
   src/app/FireController.hpp:34:    void wakeMeasured(std::chrono::steady_clock::time_point now);
   src/app/MainWindow.cpp:75:    connect(fireController, &FireController::wakeMeasured, frameMetricsCollector, &FrameMetricsCollector::observeWake);
== metricsEnabled
   src/app/FireController.cpp:53:    metricsEnabled = enabled;
   src/app/FireController.cpp:85:    if (metricsEnabled) {
   src/app/FireView.hpp:21:    void setMetricsEnabled(bool enabled) noexcept { metricsEnabled = enabled; }
   src/app/FireView.hpp:31:    bool metricsEnabled{false};
   src/app/FrameMetricsCollector.hpp:29:    [[nodiscard]] bool isEnabled() const noexcept { return metricsEnabled; }
   src/app/FrameMetricsCollector.hpp:50:    bool metricsEnabled{false};
   src/app/FireController.hpp:51:    bool metricsEnabled{false};
   src/app/ControlPanel.hpp:24:    void metricsEnabledChanged(bool enabled);
   src/app/MainWindow.cpp:57:        controlPanel, &ControlPanel::metricsEnabledChanged, frameMetricsCollector, &FrameMetricsCollector::setEnabled);
   src/app/FrameMetricsCollector.cpp:9:        metricsEnabled,
   src/app/FrameMetricsCollector.cpp:20:    if (metricsEnabled == enabled) {
   src/app/FrameMetricsCollector.cpp:27:    metricsEnabled = enabled;
   src/app/FrameMetricsCollector.cpp:28:    emit enabledChanged(metricsEnabled);
   src/app/FrameMetricsCollector.cpp:41:    if (!metricsEnabled) {
   src/app/FrameMetricsCollector.cpp:49:    if (!metricsEnabled) {
   src/app/FrameMetricsCollector.cpp:62:    if (!metricsEnabled) {
   src/app/ControlPanel.cpp:74:    connect(metricsButton, &QPushButton::toggled, this, &ControlPanel::metricsEnabledChanged);
   src/app/FireView.cpp:65:    if (metricsEnabled) {
   tests/app/ControlPanelTests.cpp:87:        &panel, &ControlPanel::metricsEnabledChanged, [&changeCount, &reportedEnabledState](const bool enabled) {
== MAXIMUM_TICKS_PER_WAKE
   src/engine/FireEngine.hpp:31:    static constexpr int MAXIMUM_TICKS_PER_WAKE = 3;
   src/engine/FireEngine.hpp:38:    FrameClock clock{SIMULATION_TICKS_PER_SECOND, MAXIMUM_TICKS_PER_WAKE};
== setStageTimingEnabled
   src/app/FireController.cpp:54:    engine.setStageTimingEnabled(enabled);
   src/engine/FireEngine.hpp:25:    void setStageTimingEnabled(bool enabled) noexcept { stageTimingEnabled = enabled; }
   tests/engine/FrameClockTests.cpp:82:    engine.setStageTimingEnabled(true);
   tests/engine/FrameClockTests.cpp:92:    engine.setStageTimingEnabled(false);
   tests/engine/FrameClockTests.cpp:118:    engine.setStageTimingEnabled(true);
```

</details>

**🔧 Tool call — `Bash`**

*Git log and presets*

```bash
git log --oneline | head -30; echo "=== presets ==="; cat CMakePresets.json | head -60
```

<details>
<summary>Result</summary>

```
5c266e0 Clean up cmake scripts and tests
8804635 Suspend app when window is minimized
b4a070f Move simulation constant down to FireEngine
aaa7856 Add StatsPanel to display metrics
94a9950 Refactor how ControlPanel works with parameters
a6a9b39 Rework metrics collectin lib
b161ed0 Introduce FireEngine as center-piece to control fire animation
dff049e Introduce FrameClock
33963aa Rewire FireController and FireView
3adc14c Inroduce renderer lib
dd80064 Introduce HeatFrame type to represent both data and dimensions
9d26f3a Add simulation tests
de17e11 Implement metrics lib and unit test
7aea7b2 Design document
24fa3eb Add build configuration with sanitizers enabled
0ef7d02 Minor clean up
844c686 Add initial README
0f4509e Switch to a fixed-step timing
3283793 Refactoring: simplify heat generation algorithm
8d52b5c Refactoring: remove wind API
34d7ffa Refactoring: remove naive fps meter
d6cea69 Refactoring: introduce FireController class
d054343 Add check-format target to justfile
993f8a8 Refactoring: extract ControlPanel and FireParameters classes.
3a6c9b3 Move simulation lib and app code into dedicated subfolders.
46ed53f Import initial draft from model "as is"
2c6ba8e Initial project scaffolding
=== presets ===
{
  "version": 6,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 25,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "cfg-debug",
      "displayName": "App Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "cfg-release",
      "displayName": "App Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "cfg-profile",
      "displayName": "App Release With Debug Info",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/profile",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "RelWithDebInfo",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "cfg-sanitize",
      "displayName": "App ASan + UBSan",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/sanitize",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON",
        "FIRE_ENABLE_ASAN": "ON",
        "FIRE_ENABLE_UBSAN": "ON"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "build-debug",
      "displayName": "App Build Debug",
      "configurePreset": "cfg-debug"
    },
    {
      "name": "build-release",
      "displayName": "App Build Release",
```

</details>

**🔧 Tool call — `Bash`**

*Trace design doc history*

```bash
git show --stat 7aea7b2 | head -20; echo "=== deleted? ==="; git log --oneline --diff-filter=D --name-only -- docs | head -20; echo "=== gitignore ==="; cat .gitignore
```

<details>
<summary>Result</summary>

```
commit 7aea7b20ec8ddacad4546de1b0063a41d3b35f10
Author: Aleksandr <alexandrv90@gmail.com>
Date:   Tue Aug 25 14:13:22 2026 +0300

    Design document

 AGENTS.md                        |   1 +
 docs/render_pipeline_redesign.md | 670 +++++++++++++++++++++++++++++++++++++++
 2 files changed, 671 insertions(+)
=== deleted? ===
b4a070f Move simulation constant down to FireEngine
docs/render_pipeline_redesign.md
=== gitignore ===
/build*/
/.cache/
/.idea/
/.vscode/
*.user
*.swp
.DS_Store
CMakeUserPresets.json
```

</details>

**🔧 Tool call — `Bash`**

*Get exact line numbers for citations*

```bash
grep -n "" src/engine/FireEngine.hpp | sed -n '13,32p'; echo "--- FireView present ---"; grep -n "" src/app/FireView.cpp | sed -n '38,50p'; echo "--- controller ---"; grep -n "" src/app/FireController.hpp | sed -n '29,40p'; echo "--- reset ---"; grep -n "" src/app/FireController.cpp | sed -n '37,46p'; echo "--- Q_OBJECT check ---"; grep -Ln "Q_OBJECT" src/app/*.hpp
```

<details>
<summary>Result</summary>

```
13:class FireEngine final {
14:public:
15:    FireEngine(std::size_t simulationWidth = SIMULATION_WIDTH, std::size_t simulationHeight = SIMULATION_HEIGHT);
16:
17:    [[nodiscard]] FrameReport advance(std::chrono::steady_clock::duration elapsed);
18:    void reset() noexcept;
19:
20:    [[nodiscard]] const PixelBuffer& frame() const noexcept { return renderer.target(); }
21:
22:    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }
23:    void setParameters(const FireParameters& parameters) noexcept;
24:
25:    void setStageTimingEnabled(bool enabled) noexcept { stageTimingEnabled = enabled; }
26:
27:private:
28:    static constexpr int SIMULATION_WIDTH = 800;
29:    static constexpr int SIMULATION_HEIGHT = 600;
30:    static constexpr int SIMULATION_TICKS_PER_SECOND = 60;
31:    static constexpr int MAXIMUM_TICKS_PER_WAKE = 3;
32:
--- FireView present ---
38:        }
39:    }
40:
41:    const std::size_t rowByteCount = pixels.width() * sizeof(Rgba32);
42:    for (std::size_t y = 0; y < pixels.height(); ++y) {
43:        const Rgba32* const source = pixels.data() + y * pixels.width();
44:        std::memcpy(frame.scanLine(static_cast<int>(y)), source, rowByteCount);
45:    }
46:
47:    update();
48:}
49:
50:QSize FireView::minimumSizeHint() const { return frame.size() / 2; }
--- controller ---
29:    void setSuspended(bool suspended);
30:
31:signals:
32:    void frameReady(FrameReport report);
33:    void frameMeasured(FrameReport report);
34:    void wakeMeasured(std::chrono::steady_clock::time_point now);
35:    void parametersChanged(FireParameters parameters);
36:    void runningChanged(bool running);
37:
38:private slots:
39:    void onWake();
40:
--- reset ---
37:void FireController::reset() {
38:    engine.reset();
39:    if (isRunning()) {
40:        lastWake = Clock::now();
41:    }
42:
43:    emit parametersChanged(engine.parameters());
44:    emit frameReady(FrameReport{});
45:}
46:
--- Q_OBJECT check ---
src/app/StatsPanel.hpp
src/app/MainWindow.hpp
```

</details>

🤝 Following project rules.

I read the whole tree (`src/`, `tests/`, `cmake/`, presets, and the commit history). Here's the architecture review.

---

## The shape of it

```
sim/      FireSimulation, FireParameters, HeatFrame      pure C++, no Qt
render/   FirePalette, PixelBuffer, FireRenderer, Viewport  pure C++, no Qt
metrics/  TimeSeriesMetric, IntervalMetric                pure C++, no Qt
engine/   FireEngine, FrameClock, FrameReport             composition root, no Qt
app/      MainWindow, FireController, FireView, ControlPanel, StatsPanel, FrameMetricsCollector   Qt
```

Dependency direction is strictly downward and enforced by CMake link scope (`fire_engine` publicly depends on `fire_simulation` + `fire_render`; Qt appears only in `src/app`). The Qt boundary rule from `AGENTS.md` holds — I found no Qt type below `app/`. That's the single most important thing this exercise is testing, and it's clean.

---

## Strong parts

**Fixed-timestep accumulator with a spiral-of-death clamp.** [FrameClock](src/engine/FrameClock.cpp:28) is textbook: accumulate elapsed, clamp to `maximumCatchUpTime`, drain into whole ticks, and — the part people usually skip — *report* the discarded time instead of silently swallowing it. Simulation rate is decoupled from wake rate. This is the correct real-time structure.

**Zero-copy data view between layers.** [HeatFrame](src/sim/HeatFrame.hpp) is a `span` + dimensions with an invariant asserted in the constructor. `FireSimulation::heat()` returns it by value; the renderer consumes it without allocating. The "data + geometry travel together" type is exactly the right abstraction here, and `row()` gives the renderer a cache-linear iteration order.

**Deterministic simulation.** Seeded xorshift32, `reset()` restores `initialSeed`. That's what makes [FireSimulationTests.cpp](tests/sim/FireSimulationTests.cpp) possible at all, and it's a deliberate choice — not an accident.

**The palette is a 256-entry LUT, not per-pixel math.** 1 KB, stays in L1, `u8 → Rgba32` in one indexed load. For a CPU renderer this is the right call.

**Testability is the standout quality.** Every non-Qt layer is unit-tested, and the Qt widgets are tested headless via `QT_QPA_PLATFORM=offscreen` wired through the `fire_add_test(... ENVIRONMENT ...)` helper. `FireEngine` accepts 8×6 dimensions so engine tests run instantly. Someone thought about the test seam while designing, not after.

**The history shows pruning, not accretion.** `remove wind API`, `remove naive fps meter`, `simplify heat generation algorithm`, `Switch to a fixed-step timing`. The design doc was written, implemented, then deleted. That's a good signal — most of the speculative code that would normally be here has already been removed.

---

## Weak parts — real-time graphics

**1. The frame data is copied twice and neither copy is necessary.**

`FireRenderer::render()` writes into an owned `PixelBuffer` ([FireRenderer.cpp:19](src/render/FireRenderer.cpp:19)), then `FireView::present()` memcpys that buffer row-by-row into a `QImage` ([FireView.cpp:41](src/app/FireView.cpp:41)). At 800×600×4 that's ~1.9 MB of pure copy per frame, and the `QImage` is a permanently resident duplicate of a buffer that already exists.

The Qt-purity rule is what forces this, but it doesn't have to. The fix that preserves the boundary: give `FireRenderer` a *sink* rather than an owned target —

```cpp
void render(const HeatFrame& heat, std::span<Rgba32> destination, std::size_t rowStride);
```

`FireView` then passes `QImage::bits()` / `bytesPerLine() / 4`. The render library still knows nothing about Qt; the copy disappears; `PixelBuffer` becomes the *test* target rather than the production one. Note the stride parameter is load-bearing — `QImage` scanlines are 4-byte-aligned and padded, which `PixelBuffer` has no concept of today.

**2. `present()` does the upload even for frames that are never painted.**

`present()` memcpys and then calls `update()`, which only *schedules* a repaint and coalesces. If the compositor is behind, several `present()` calls land between paints and you pay for uploads nobody sees. The pull model is cleaner: `FireView` holds a reference to the engine's buffer and does the upload inside `paintEvent`, with `present()` reduced to `update()`. (Safe here — everything is on the GUI thread — but see #4.)

**3. Nothing is synchronized to the display.**

`WAKE_INTERVAL_MILLISECONDS = 16` in [FireController.hpp](src/app/FireController.hpp) and `SIMULATION_TICKS_PER_SECOND = 60` in [FireEngine.hpp:30](src/engine/FireEngine.hpp:30) are both hard-coded, and neither consults `QScreen::refreshRate()`. On a 120 Hz or ProMotion display you get a 62.5 Hz timer feeding a 60 Hz sim feeding an unsynced raster blit — beat-frequency judder. The `FrameClock` design is *ready* for this (it takes `ticksPerSecond` as a constructor argument), but nothing ever supplies a real value. That's a one-line fix with a real architectural point behind it: presentation rate is a display property, not a compile-time constant.

**4. There is no frame-buffer ownership model, so the threading seam isn't actually there.**

`FireEngine::frame()` returns a reference to the *live* render target ([FireEngine.hpp:20](src/engine/FireEngine.hpp:20)). Everything works because sim, shade, and paint all run on the GUI thread. But this means moving simulation off-thread later isn't a refactor — it's an interface change, because there's no double-buffer or handoff.

I would **not** add threading here; that's premature for a demo. But the honest framing is that `FireEngine::advance()` is *almost* a clean producer (`elapsed → FrameReport`, self-contained state) and one design decision — returning an owned/swapped frame instead of a live reference — is what separates it from being one.

**5. Upscaling cost scales with window size, not simulation size.**

`SmoothPixmapTransform` bilinear over the full widget rect every paint, in Qt's raster engine. The *architecture* around it is right — [Viewport.hpp](src/render/Viewport.hpp) keeps the aspect-fit math as a pure, testable, Qt-free function and only the actual blit is delegated to `QPainter`. Good split. Two notes: the sim resolution is fixed and never follows the widget, so the whole cost is in scaling; and `Viewport.hpp` lives in `render/` but is a presentation concern used only by `app/` — arguably misplaced.

**6. No top-level error semantics.**

`FireSimulation` and `PixelBuffer` throw `invalid_argument` / `length_error`, and `FireEngine` is a by-value member of `FireController`, constructed inside `MainWindow`'s constructor. A throw there propagates out of an uncaught `main()` → `terminate`. It can't fire in production because the dimensions are constants — but `AGENTS.md` asks for deliberate error semantics, and "constructors throw, nobody catches" isn't one.

---

## Speculative code

The pruning commits removed most of it. What's left:

| Item | Verdict |
|---|---|
| `FirePalette::fromStops` public + `FireRenderer::setPalette` ([FireRenderer.hpp:11](src/render/FireRenderer.hpp:11)) | **Speculative.** ~30 lines of stop-interpolation machinery and a runtime setter to produce exactly one hard-coded palette. No UI path changes it; only tests call `setPalette`. Either expose a palette selector in `ControlPanel` (2 lines, and it justifies the whole thing) or make `fromStops` private and drop `setPalette`. |
| `FrameReport::elapsed` and `::discardedTime` | **Speculative.** Neither is consumed by any production code — `StatsPanel` displays neither. Both exist only to be asserted in [FrameClockTests.cpp](tests/engine/FrameClockTests.cpp). `discardedTime` in particular deserves to be shown; a "dropped 12 ms" row is exactly the diagnostic that field is for. Show them or drop them. |
| `FrameMetricsSnapshot::enabled` | **Redundant.** `StatsPanel` never reads it — it calls `metricsCollector.isEnabled()` directly. Only tests touch it. |
| `HeatFrame::cells()` | Test-only accessor, but it's two lines and it's how `row()` is defined conceptually. Fine. |
| `FireController::isSuspended()` | Test-only. Acceptable — it's the observable half of a state machine. |
| `FireEngine` default constructor arguments ([FireEngine.hpp:15](src/engine/FireEngine.hpp:15)) | **Odd, not speculative.** The defaults reference `SIMULATION_WIDTH`/`SIMULATION_HEIGHT`, which are **private** — legal, but callers can't name them. And because both have defaults the constructor isn't `explicit`, so `FireEngine e = 800;` compiles. Make it `explicit` and either promote the constants or drop the defaults. |

---

## Abstractions: over, under, and inconsistent

**`MetricsClock` doesn't insulate anything.** [MetricsClock.hpp](src/metrics/MetricsClock.hpp) is a five-line file containing one `using MetricsClock = std::chrono::steady_clock;`. A thin alias is defensible as a documented swap point — except the engine layer doesn't honor it: `FrameReport` and `FrameStageTimings` use `std::chrono::steady_clock` directly, and those durations are then fed into `TimeSeriesMetric::record(MetricsClock::duration)`. It compiles only because the two are the same type. So the abstraction provides *zero* actual insulation while still costing a file. Either use it consistently across `engine/` or delete it and use `steady_clock`.

**`IntervalMetric` over `TimeSeriesMetric` is correctly sized.** 17 lines that turn timestamps into deltas and delegate. This is what a good small abstraction looks like — don't touch it.

**The app layer is not a library, and that costs you.** [src/app/CMakeLists.txt](src/app/CMakeLists.txt) lists `FrameMetricsCollector.cpp` in the executable **and** in three separate test targets, each recompiling it with its own AUTOMOC pass. `StatsPanel.cpp`, `ControlPanel.cpp`, `FireController.cpp` are duplicated the same way. Extract a `fire_app` static library (Qt-linked, `AUTOMOC`), leave `fire_demo` as `main.cpp` + link, and every test target collapses to one source file plus a dependency. This is the one place the build genuinely contradicts the "prevent duplication" rule.

**The CMake wrapper functions are heavy but earn their keep — barely.** ~100 lines of `cmake_parse_arguments` + `FATAL_ERROR` validation for 4 libraries and 6 tests is a high ratio. For a normal project I'd call it over-abstracted. For *this* project it's a reasonable signal (consistent warnings, sanitizers, C++ standard, output dirs applied uniformly and impossible to forget). I'd keep it. It's the kind of thing that reads as competence rather than ceremony, as long as you can defend the ratio.

**`Q_OBJECT` is inconsistently applied.** `FireView`, `FireController`, `ControlPanel`, `FrameMetricsCollector` have it; [MainWindow.hpp](src/app/MainWindow.hpp) and [StatsPanel.hpp](src/app/StatsPanel.hpp) don't. It happens to work — connecting *to* a sender's signal with `this` as context object doesn't require the receiver to have a metaobject, and `findChild` walks children's metaobjects. But it's an unexplained inconsistency in a codebase that is otherwise very consistent, and it costs nothing to fix under AUTOMOC.

**Two ways to set parameters.** `FireSimulation::parameters()` returns a mutable reference ([FireSimulation.hpp:24](src/sim/FireSimulation.hpp:24)) *and* `FireEngine::setParameters` exists, implemented as `simulation.parameters() = parameters;`. The comment defends the mutable accessor well (self-clamping type, no derived state), and I agree with the reasoning — but nothing except `FireEngine` uses it, so the public mutable accessor is a second path to the same state with no caller.

**A synthetic signal payload.** [FireController.cpp:44](src/app/FireController.cpp:44) emits `frameReady(FrameReport{})` — a fabricated report — after a reset. It works because `MainWindow` ignores the payload entirely ([MainWindow.cpp:67](src/app/MainWindow.cpp:67) takes `const FrameReport&` and never reads it). No consumer of `frameReady` ever uses the report. Make it `frameReady()` and let `frameMeasured(FrameReport)` carry the data; the fake object disappears and the two signals get distinct meanings.

**One implicit lifetime invariant.** `StatsPanel` holds `const FrameMetricsCollector&`. It's safe only because `centralWidget` is created before `frameMetricsCollector` in `MainWindow`'s constructor, so Qt destroys the panel's subtree first. Reorder those two lines and the reference outlives its referent. Nothing documents this. Passing a pointer with a `QPointer`, or simply making the collector a `MainWindow` value member declared before the widget tree, makes the invariant structural.

---

## What I'd change, ranked

1. **Render into a caller-supplied span + stride.** Kills both copies, keeps the Qt boundary, and is the strongest architectural answer in the whole review.
2. **Extract a `fire_app` library.** Removes real, mechanical build duplication.
3. **Resolve the palette question.** Expose it in the UI or make it private. Right now it's the clearest piece of unused generality.
4. **Drive tick rate from `QScreen::refreshRate()`.** `FrameClock` is already parameterized for it.
5. **`frameReady()` without a payload; surface `discardedTime` in `StatsPanel`.** Deletes a fake object and makes a dead field earn its place.
6. **`Q_OBJECT` on `MainWindow`/`StatsPanel`; `explicit` on `FireEngine`; use or delete `MetricsClock`.** Small consistency fixes.

**Overall:** the layering, the fixed-timestep clock, the zero-copy heat view, and the test seam are genuinely well done and are what the exercise is asking about. The weakest area is the render→present handoff — where the Qt boundary is currently paid for in copies rather than designed around — and that's also where the highest-value fix lives.

I haven't changed anything. Want me to implement any of these, or publish this as a shareable page?

## 👤 User  
*2026-08-26 16:03:57.511 UTC*

I'm willing to keep additional memcpy for purer abstraction for now.
However, I'm thinking of dissolving renderer lib - Viewport -> FireView method, FireRenderer ->FireEngine,  FirePalette ->engine lib. Motivation - it's not an abstract rendering engine, it's not really possible to create another renderer with current abstraction anyway. So there's no point in keeping it.

## 🤖 Assistant  
*2026-08-26 16:04:58.123 UTC*

Let me check two things before I answer — what the render target actually declares, and whether `fitPreservingAspect` is currently under test.

**🔧 Tool call — `Bash`**

*Read render CMake and render tests*

```bash
echo "=== src/render/CMakeLists.txt ==="; cat src/render/CMakeLists.txt; echo; echo "=== RenderTests.cpp ==="; cat tests/render/RenderTests.cpp
```

<details>
<summary>Result</summary>

```
=== src/render/CMakeLists.txt ===
fire_add_library(fire_render
    SOURCES
        FirePalette.cpp
        FirePalette.hpp
        FireRenderer.cpp
        FireRenderer.hpp
        PixelBuffer.cpp
        PixelBuffer.hpp
        Viewport.hpp
    PUBLIC_DEPENDENCIES
        fire_simulation
)

fire_add_test(fire_render_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/render/RenderTests.cpp"
    DEPENDENCIES
        fire_render
)

=== RenderTests.cpp ===
#include "render/FirePalette.hpp"
#include "render/FireRenderer.hpp"
#include "render/PixelBuffer.hpp"
#include "render/Viewport.hpp"
#include "sim/HeatFrame.hpp"
#include "tests_common.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string_view>

namespace {
using fire_tests::check;

void checkColor(const Rgba32 actual, const Rgba32 expected, const std::string_view message) {
    if (actual == expected) {
        return;
    }

    std::cerr << "FAILED: " << message << " (expected 0x" << std::hex << expected << ", got 0x" << actual << std::dec
              << ")\n";
    fire_tests::recordFailure();
}

void testPixelBufferGeometry() {
    PixelBuffer buffer;
    check(buffer.width() == 0 && buffer.height() == 0, "a new pixel buffer has empty geometry");

    buffer.resize(3, 2);
    check(buffer.width() == 3, "pixel buffer reports its width");
    check(buffer.height() == 2, "pixel buffer reports its height");
    check(buffer.row(0).size() == 3, "pixel buffer exposes complete rows");
    check(buffer.row(1).data() == buffer.data() + 3, "pixel buffer locates its final row");

    buffer.row(0)[2] = 0xFF010203u;
    checkColor(buffer.data()[2], 0xFF010203u, "pixel buffer rows expose writable storage");

    bool rejectedOversizedGeometry = false;
    try {
        buffer.resize(std::numeric_limits<std::size_t>::max(), 2);
    } catch (const std::length_error&) {
        rejectedOversizedGeometry = true;
    }
    check(rejectedOversizedGeometry, "pixel buffer rejects dimensions whose area cannot be represented");
    check(buffer.width() == 3 && buffer.height() == 2, "a rejected resize preserves pixel buffer geometry");
}

void testClassicPalette() {
    const FirePalette palette = FirePalette::classic();

    checkColor(palette[0], 0xFF000000u, "classic palette begins with black");
    checkColor(palette[32], 0xFF2D0000u, "classic palette retains its first red stop");
    checkColor(palette[136], 0xFFFF2D00u, "classic palette retains its orange stop");
    checkColor(palette[232], 0xFFFFF550u, "classic palette retains its yellow stop");
    checkColor(palette[255], 0xFFFFFFFFu, "classic palette ends with white");
}

void testPaletteInterpolation() {
    constexpr std::array stops{
        PaletteStop{0, 0, 10, 20},
        PaletteStop{2, 10, 20, 30},
        PaletteStop{255, 255, 255, 255},
    };
    const FirePalette palette = FirePalette::fromStops(stops);

    checkColor(palette[0], 0xFF000A14u, "custom palette includes its first stop");
    checkColor(palette[1], 0xFF050F19u, "custom palette linearly interpolates channels");
    checkColor(palette[2], 0xFF0A141Eu, "custom palette includes an interior stop");
    checkColor(palette[255], 0xFFFFFFFFu, "custom palette includes its final stop");
}

void testPaletteValidation() {
    constexpr std::array<PaletteStop, 0> noStops{};
    constexpr std::array missingFirstStop{PaletteStop{1, 0, 0, 0}, PaletteStop{255, 255, 255, 255}};
    constexpr std::array missingLastStop{PaletteStop{0, 0, 0, 0}, PaletteStop{254, 255, 255, 255}};
    constexpr std::array duplicateStop{
        PaletteStop{0, 0, 0, 0},
        PaletteStop{20, 20, 20, 20},
        PaletteStop{20, 30, 30, 30},
        PaletteStop{255, 255, 255, 255},
    };
    constexpr std::array descendingStops{
        PaletteStop{0, 0, 0, 0},
        PaletteStop{30, 30, 30, 30},
        PaletteStop{20, 20, 20, 20},
        PaletteStop{255, 255, 255, 255},
    };

    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(noStops)); },
                                                   "palette rejects an empty stop list");
    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(missingFirstStop)); },
                                                   "palette requires its first stop at index zero");
    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(missingLastStop)); },
                                                   "palette requires its final stop at index 255");
    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(duplicateStop)); },
                                                   "palette rejects duplicate stop indices");
    fire_tests::checkThrows<std::invalid_argument>([&] { static_cast<void>(FirePalette::fromStops(descendingStops)); },
                                                   "palette rejects descending stop indices");
}

void testRenderer() {
    constexpr std::array heatCells{
        std::uint8_t{0},
        std::uint8_t{1},
        std::uint8_t{2},
        std::uint8_t{255},
    };
    constexpr std::array paletteStops{
        PaletteStop{0, 0, 0, 0},
        PaletteStop{255, 255, 0, 255},
    };
    FireRenderer renderer{FirePalette::fromStops(paletteStops)};

    renderer.render(HeatFrame{heatCells, 2, 2});
    const PixelBuffer& target = renderer.target();
    check(target.width() == 2 && target.height() == 2, "renderer sizes its target from the heat frame");
    checkColor(target.data()[0], 0xFF000000u, "renderer shades zero heat");
    checkColor(target.data()[1], 0xFF010001u, "renderer shades low heat");
    checkColor(target.data()[2], 0xFF020002u, "renderer shades heat through its palette");
    checkColor(target.data()[3], 0xFFFF00FFu, "renderer shades maximum heat");

    const Rgba32* const originalStorage = target.data();
    renderer.render(HeatFrame{heatCells, 2, 2});
    check(renderer.target().data() == originalStorage, "renderer reuses storage when frame geometry is unchanged");

    constexpr std::array replacementStops{
        PaletteStop{0, 255, 255, 255},
        PaletteStop{255, 0, 0, 0},
    };
    renderer.setPalette(FirePalette::fromStops(replacementStops));
    renderer.render(HeatFrame{heatCells, 4, 1});
    check(renderer.target().width() == 4 && renderer.target().height() == 1,
          "renderer resizes its target when frame geometry changes");
    checkColor(renderer.target().data()[0], 0xFFFFFFFFu, "renderer uses a replacement palette");
    checkColor(renderer.target().data()[3], 0xFF000000u, "replacement palette covers maximum heat");
}

void checkRect(const FitRect& actual, const FitRect& expected, const std::string_view message) {
    check(actual.x == expected.x && actual.y == expected.y && actual.width == expected.width &&
              actual.height == expected.height,
          message);
}

void testViewport() {
    checkRect(fitPreservingAspect(800, 600, 640, 480), {0, 0, 800, 600}, "viewport preserves a matching aspect");
    checkRect(fitPreservingAspect(100, 100, 16, 9), {0, 22, 100, 56}, "viewport letterboxes a wide source");
    checkRect(fitPreservingAspect(100, 100, 9, 16), {22, 0, 56, 100}, "viewport pillarboxes a tall source");
    checkRect(fitPreservingAspect(101, 100, 2, 1), {0, 25, 101, 50}, "viewport centres an odd-sized fit");
    checkRect(fitPreservingAspect(0, 100, 16, 9), {}, "viewport rejects an empty available width");
    checkRect(fitPreservingAspect(100, 100, 0, 9), {}, "viewport rejects an empty source width");
    checkRect(fitPreservingAspect(-1, 100, 16, 9), {}, "viewport rejects negative dimensions");
    checkRect(fitPreservingAspect(std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max(),
                                  std::numeric_l
```

*[truncated, 418 more characters]*

</details>

Facts that bear on this: `fire_render` **publicly** depends on `fire_simulation` (src/render/CMakeLists.txt:10), and `testViewport` in [RenderTests.cpp:150](tests/render/RenderTests.cpp:150) is 8 assertions including an `INT_MAX` overflow case.

## I agree — but your stated reason is the weaker one

"You can't create another renderer with this abstraction" is an argument against having an *interface*. It isn't, by itself, an argument against having a *library target* — targets exist to enforce dependency direction, not only to enable substitution.

The stronger version of your argument is sitting in the CMake: **`fire_render` enforces nothing.** It publicly depends on `fire_simulation`, and `fire_engine` publicly depends on both. So every consumer that can see `fire_render` can already see everything `fire_render` can see. The boundary constrains no one. It's a target with one consumer, one implementation, and zero isolation value — that's a directory wearing a library's clothes.

What you actually give up: nothing today stops palette code from `#include "engine/FrameClock.hpp"` after the merge. In a five-file module that's a non-risk. Say that out loud and the decision is airtight.

There's also a consistency point worth making explicitly, since you just decided to keep the memcpy: the one remaining reason to keep `FireRenderer` as a separate object was to hand it an external destination (`renderInto(span, stride)`). You've declined that. So the renderer's separability has no surviving purpose — the two decisions support each other rather than being two independent simplifications.

## Two refinements

**1. `FireRenderer` → a free function, not a private method.**

`FireEngine` already owns both responsibilities in its public contract — `advance()` times `simulate` and `shade` as distinct stages, `setStageTimingEnabled` exists to measure both, `frame()` returns the render target. `FireRenderer` was never a seam; [FireEngine::shade()](src/engine/FireEngine.cpp:48) is a one-line forward. Dissolving it makes the honest structure visible. Agreed.

But watch what it costs: `testRenderer()` currently feeds synthetic heat — `HeatFrame{heatCells, 2, 2}` — and asserts exact output colors. Fold the loop into a private `FireEngine::shade()` and you can no longer inject heat; you can only observe whatever the simulation happens to produce. You'd lose the exact-color tests.

Keep them for free by making it a free function in the engine module instead of a method:

```cpp
void shade(const HeatFrame& heat, const FirePalette& palette, PixelBuffer& target);
```

`FireEngine` holds `FirePalette palette; PixelBuffer renderTarget;` and calls it. The class disappears, the indirection disappears, and the test still drives it with synthetic input. Also drop `setPalette` while you're in there — it was already unused outside tests, and this is the natural moment.

**2. Keep `fitPreservingAspect` a free function — relocate it, don't absorb it.**

This is the one part I'd push back on. It's a pure function of four `int`s; as a `FireView` method it gains nothing, because the caller already has every input. What it *loses* is the best test in the render suite — as a private method of a `QWidget`, testing it needs a `QApplication`, the offscreen platform plugin, and a friend declaration or public exposure. Realistically you'd delete the test, and the `int64_t` intermediate that prevents overflow at `INT_MAX` would stop being pinned by anything.

Move the file to `src/app/Viewport.hpp` and leave it a free function. It's header-only, so the test target costs nothing but `INCLUDE_SOURCE_ROOT` — no dependency on whether you extract a `fire_app` library.

## Where that lands

```
sim/      FireSimulation, FireParameters, HeatFrame          [fire_simulation]
engine/   FireEngine, FrameClock, FrameReport,
          FirePalette, PixelBuffer, shade()                  [fire_engine]
metrics/  TimeSeriesMetric, IntervalMetric, ...              [fire_metrics]
app/      MainWindow, FireController, FireView, ControlPanel,
          StatsPanel, FrameMetricsCollector, Viewport.hpp    [fire_demo]
```

Four targets become three-plus-exe. Layer depth is unchanged (`sim → engine → app`), file count is unchanged, one target and one indirection are gone.

The grab-bag objection people will reach for — "engine is now a pile" — doesn't hold, and it's worth having the answer ready: the engine's contract is *heat in, paced RGBA frame out*. `FrameClock` paces, `FireEngine` orchestrates, `FrameReport` reports, `PixelBuffer` **is** the frame, `FirePalette` is how heat becomes color. Every file is constitutive of that one sentence. Five cohesive concepts is a module, not a pile.

Two loose ends: you didn't say where `PixelBuffer` goes — it has to be engine, since it's `FireEngine::frame()`'s return type. And `RenderTests.cpp` splits: palette/pixel-buffer/shade cases to `tests/engine/`, viewport cases to `tests/app/`.

`FirePalette::fromStops` stays exactly as speculative as it was — the merge neither helps nor hurts it. Still worth either wiring a palette selector into `ControlPanel` or accepting it as tested-but-unused.

Want me to make these changes?
