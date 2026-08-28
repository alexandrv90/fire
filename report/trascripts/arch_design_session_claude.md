# Architecture review: extendability and decomposition

## 👤 User  
*2026-08-25 15:19:31.665 UTC*

I want you to run architecture review of current implementation. This task has nothing to do with runtime performance or building process. Do not run benchmarks, modify code or build anything. Pure review.

I want you to focus on system extendability and how  decomposition is currently implemented.
E.g. how would it react if we are going to add performance monitoring?

## 🤖 Assistant  
*2026-08-25 15:19:33.620 UTC*

I'll start by exploring the codebase structure to understand what we're working with.

**🔧 Tool call — `Bash`**

*List repo structure*

```bash
ls -la && echo "---TREE---" && find . -type f -not -path './.git/*' | head -100
```

<details>
<summary>Result</summary>

```
total 80
drwxr-xr-x@ 14 User  staff   448 Aug 25 18:16 .
drwxr-xr-x  33 User  staff  1056 Aug 25 14:27 ..
-rw-r--r--@  1 User  staff  6148 Aug 24 09:10 .DS_Store
-rw-r--r--@  1 User  staff   195 Aug 24 08:56 .clang-format
drwxr-xr-x@ 15 User  staff   480 Aug 25 18:16 .git
-rw-r--r--@  1 User  staff    81 Aug 24 08:56 .gitignore
-rw-r--r--@  1 User  staff  4285 Aug 25 17:57 AGENTS.md
-rw-r--r--@  1 User  staff  1856 Aug 25 18:16 CMakeLists.txt
-rw-r--r--@  1 User  staff  1859 Aug 25 17:56 CMakePresets.json
-rw-r--r--@  1 User  staff  1255 Aug 25 17:56 README.md
drwxr-xr-x@  6 User  staff   192 Aug 25 17:56 build
drwxr-xr-x@  3 User  staff    96 Aug 25 17:56 cmake
-rw-r--r--@  1 User  staff   960 Aug 25 17:56 justfile
drwxr-xr-x@  5 User  staff   160 Aug 25 17:54 src
---TREE---
./CMakeLists.txt
./.DS_Store
./justfile
./README.md
./CMakePresets.json
./.gitignore
./.clang-format
./AGENTS.md
./cmake/Sanitizers.cmake
./src/main.cpp
./build/sanitize/compile_commands.json
./build/sanitize/.ninja_deps
./build/sanitize/cmake_install.cmake
./build/sanitize/build.ninja
./build/sanitize/libfire_simulation.a
./build/sanitize/CMakeCache.txt
./build/sanitize/libfire_metrics.a
./build/sanitize/.ninja_log
./build/release/compile_commands.json
./build/release/.ninja_deps
./build/release/cmake_install.cmake
./build/release/build.ninja
./build/release/libfire_simulation.a
./build/release/CMakeCache.txt
./build/release/libfire_metrics.a
./build/release/.ninja_log
./build/profile/compile_commands.json
./build/profile/.ninja_deps
./build/profile/cmake_install.cmake
./build/profile/build.ninja
./build/profile/libfire_simulation.a
./build/profile/CMakeCache.txt
./build/profile/.ninja_log
./build/debug/compile_commands.json
./build/debug/.ninja_deps
./build/debug/cmake_install.cmake
./build/debug/build.ninja
./build/debug/libfire_simulation.a
./build/debug/CMakeCache.txt
./build/debug/.ninja_log
./src/app/FireWidget.cpp
./src/app/FireController.cpp
./src/app/MainWindow.hpp
./src/app/ControlPanel.hpp
./src/app/MainWindow.cpp
./src/app/FireController.hpp
./src/app/ControlPanel.cpp
./src/app/FireWidget.hpp
./src/sim/FireParameters.hpp
./src/sim/FireSimulation.cpp
./src/sim/FireParameters.cpp
./src/sim/FireSimulation.hpp
./build/debug/.qt/QtDeploySupport.cmake
./build/debug/.qt/QtDeployTargets.cmake
./build/debug/CMakeFiles/cmake.check_cache
./build/debug/CMakeFiles/CMakeConfigureLog.yaml
./build/debug/CMakeFiles/rules.ninja
./build/debug/CMakeFiles/InstallScripts.json
./build/debug/CMakeFiles/TargetDirectories.txt
./build/debug/CMakeFiles/clean_additional.cmake
./build/debug/classic_fire_autogen/mocs_compilation.cpp
./build/debug/classic_fire_autogen/deps
./build/debug/classic_fire_autogen/moc_predefs.h
./build/debug/classic_fire_autogen/timestamp
./build/sanitize/.qt/QtDeploySupport.cmake
./build/sanitize/.qt/QtDeployTargets.cmake
./build/sanitize/CMakeFiles/cmake.check_cache
./build/sanitize/CMakeFiles/CMakeConfigureLog.yaml
./build/sanitize/CMakeFiles/rules.ninja
./build/sanitize/CMakeFiles/InstallScripts.json
./build/sanitize/CMakeFiles/TargetDirectories.txt
./build/sanitize/CMakeFiles/clean_additional.cmake
./build/sanitize/classic_fire_autogen/mocs_compilation.cpp
./build/sanitize/classic_fire_autogen/deps
./build/sanitize/classic_fire_autogen/moc_predefs.h
./build/sanitize/classic_fire_autogen/timestamp
./build/release/.qt/QtDeploySupport.cmake
./build/release/.qt/QtDeployTargets.cmake
./build/release/CMakeFiles/cmake.check_cache
./build/release/CMakeFiles/CMakeConfigureLog.yaml
./build/release/CMakeFiles/rules.ninja
./build/release/CMakeFiles/InstallScripts.json
./build/release/CMakeFiles/TargetDirectories.txt
./build/release/CMakeFiles/clean_additional.cmake
./build/release/classic_fire_autogen/mocs_compilation.cpp
./build/release/classic_fire_autogen/deps
./build/release/classic_fire_autogen/moc_predefs.h
./build/release/classic_fire_autogen/timestamp
./build/profile/.qt/QtDeploySupport.cmake
./build/profile/.qt/QtDeployTargets.cmake
./build/profile/CMakeFiles/cmake.check_cache
./build/profile/CMakeFiles/CMakeConfigureLog.yaml
./build/profile/CMakeFiles/rules.ninja
./build/profile/CMakeFiles/InstallScripts.json
./build/profile/CMakeFiles/TargetDirectories.txt
./build/profile/CMakeFiles/clean_additional.cmake
./build/profile/classic_fire_autogen/mocs_compilation.cpp
./build/profile/classic_fire_autogen/deps
./build/profile/classic_fire_autogen/moc_predefs.h
./build/profile/classic_fire_autogen/timestamp
```

</details>

Small codebase — I'll read all of it.

**🔧 Tool call — `Bash`**

*Count lines of source files*

```bash
wc -l src/*/*.?pp src/main.cpp CMakeLists.txt AGENTS.md README.md
```

<details>
<summary>Result</summary>

```
      75 src/app/ControlPanel.cpp
      24 src/app/ControlPanel.hpp
      72 src/app/FireController.cpp
      50 src/app/FireController.hpp
     108 src/app/FireWidget.cpp
      28 src/app/FireWidget.hpp
      49 src/app/MainWindow.cpp
      12 src/app/MainWindow.hpp
      11 src/sim/FireParameters.cpp
      24 src/sim/FireParameters.hpp
      82 src/sim/FireSimulation.cpp
      36 src/sim/FireSimulation.hpp
      17 src/main.cpp
      67 CMakeLists.txt
      66 AGENTS.md
      44 README.md
     765 total
```

</details>

**🔧 Tool call — `Bash`**

*Read docs and build config*

```bash
for f in README.md AGENTS.md CMakeLists.txt src/main.cpp; do echo "════════ $f ════════"; cat -n "$f"; done
```

<details>
<summary>Result</summary>

````
════════ README.md ════════
     1	# Fire Demo
     2	
     3	A C++20 and Qt Widgets demo of the classic procedural fire effect, with live simulation controls.
     4	
     5	## Prerequisites
     6	
     7	- CMake 3.25+
     8	- Ninja
     9	- Qt 5 or Qt 6 with Widgets
    10	- A C++20 compiler
    11	
    12	## Build and run
    13	
    14	```sh
    15	cmake --preset cfg-release
    16	cmake --build --preset build-release --target run_app
    17	```
    18	
    19	### Optional: build with `just` tool if available
    20	
    21	```sh
    22	just run-release
    23	```
    24	
    25	## Sanitizer build
    26	
    27	Clang and GCC builds can enable AddressSanitizer and UndefinedBehaviorSanitizer together. The sanitizer configuration instruments both the simulation library and the Qt application code; installed Qt libraries are not rebuilt with instrumentation.
    28	
    29	```sh
    30	cmake --preset cfg-sanitize
    31	cmake --build --preset build-sanitize
    32	
    33	ASAN_OPTIONS=halt_on_error=1 \
    34	UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
    35	cmake --build --preset build-sanitize --target run_app
    36	```
    37	
    38	With `just` installed, configure, build, and run the sanitizer build with:
    39	
    40	```sh
    41	just run-sanitize
    42	```
    43	
    44	Sanitizers only check code paths exercised while the application runs. Treat any sanitizer diagnostic as a failure, fix the first project-owned stack frame, then rebuild and rerun. Sanitized executables are diagnostic artifacts and should not be distributed as release builds.
════════ AGENTS.md ════════
     1	# AGENTS.md
     2	
     3	Operational guide for LLM coding tools working in this repository. Read this first.
     4	
     5	If a rule conflicts with a task, stop and surface it. Do not silently work around the rules.
     6	
     7	## Acknowledgment
     8	
     9	To confirm that you have read necessary files and will respect discovered rules for the duration of the task, print the following in the beggining of each session:
    10	
    11	> 🤝 Following project rules.
    12	
    13	If you cannot honor a rule for any reason, surface that explicitly instead of printing the acknowledgment.
    14	
    15	---
    16	
    17	## Doc map
    18	
    19	Read these before touching code in the corresponding area.
    20	
    21	| If you're working on…                                          | Read first                                        |
    22	| -------------------------------------------------------------- | ------------------------------------------------- |
    23	| Anything                                                       | `README.md`                                       |
    24	
    25	---
    26	
    27	## Coding rules
    28	
    29	Authoritative coding rules for current project. Apply to all contributors — human and LLM.
    30	
    31	1. C++20 baseline
    32	2. **Naming.** `lowerCamelCase` for variables and functions, `PascalCase` for types, `SCREAMING_SNAKE_CASE` for constants, `PascalCase` for filenames matching the primary type
    33	3. **Simulation boundary.** The simulation library must remain pure C++; no Qt type may ever cross this boundary.
    34	
    35	---
    36	
    37	## Implementation approach
    38	
    39	- Treat each task as an incremental change to the whole system, not as an isolated patch. Inspect the surrounding code, documented architecture, ownership model and likely extension points before choosing an interface or class design.
    40	- Prefer the smallest coherent design over the smallest diff. Keep the implementation limited to the task, but do not sacrifice clean boundaries, accurate abstractions or future compatibility to finish it quickly.
    41	- Prevent duplication. Search for existing implementations and shared concepts before adding code. When functionality is inaccessible or poorly placed, make a focused refactoring that creates an appropriate shared abstraction instead of copying it.
    42	- Follow modern C++ best practices: use RAII, explicit ownership, value semantics, strong types and standard-library facilities. Avoid owning raw pointers and manual resource cleanup.
    43	- Encapsulate C libraries and other low-level APIs behind narrow C++ abstractions. Resource ownership alone is not sufficient: the abstraction should also enforce valid lifecycle transitions, preserve invariants and keep implementation details from leaking into callers.
    44	- Match documented interfaces and responsibilities by behavior, not merely by class name or file location. Do not introduce a type whose API conflicts with the architectural role its name implies.
    45	- Define error and partial-success semantics deliberately. Avoid ad hoc combinations of status flags, strings and side-channel error collections when a clearer result model is possible.
    46	- Before implementation, identify existing reusable code, resource ownership, object lifecycle and error propagation. After implementation, review the change for duplication, unnecessary repeated work and architectural drift.
    47	- Do not over-engineer speculative features. Broader design awareness should improve the current change without introducing unrelated frameworks or abstractions.
    48	
    49	---
    50	
    51	## Formatting (end of task)
    52	
    53	After all code changes are complete and verified (build/tests pass), run **clang-format** on every C/C++ source and header file you modified in the task. Do this as the **last** step before declaring the work done — not at the start, and not in the middle of edits you still intend to change.
    54	
    55	- Use the repo config: `.clang-format` at the repository root.
    56	- Format only files you touched (e.g. `clang-format -i path/to/file.cpp`). Do not reformat unrelated files.
    57	- If `clang-format` is not installed, say so explicitly instead of skipping silently.
    58	
    59	---
    60	
    61	## When in doubt
    62	
    63	- Ask before inventing. Don't guess function signatures, file paths, library APIs, or build invocations. If something isn't in the docs or the code, it doesn't exist yet.
    64	- If two docs disagree, surface the conflict instead of guessing which one is right.
    65	- If a rule conflicts with the task, stop. Don't silently work around the rule.
    66	
════════ CMakeLists.txt ════════
     1	cmake_minimum_required(VERSION 3.25)
     2	
     3	project(ClassicFireDemo VERSION 1.0.0 LANGUAGES CXX)
     4	
     5	set(CMAKE_CXX_STANDARD 20)
     6	set(CMAKE_CXX_STANDARD_REQUIRED ON)
     7	set(CMAKE_CXX_EXTENSIONS OFF)
     8	
     9	include(cmake/Sanitizers.cmake)
    10	
    11	find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    12	find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    13	
    14	add_library(fire_simulation
    15	    src/sim/FireParameters.cpp
    16	    src/sim/FireParameters.hpp
    17	    src/sim/FireSimulation.cpp
    18	    src/sim/FireSimulation.hpp
    19	)
    20	
    21	target_include_directories(fire_simulation PUBLIC src)
    22	target_compile_features(fire_simulation PUBLIC cxx_std_20)
    23	fire_enable_sanitizers(fire_simulation)
    24	
    25	add_executable(classic_fire
    26	    src/app/ControlPanel.cpp
    27	    src/app/ControlPanel.hpp
    28	    src/app/FireController.cpp
    29	    src/app/FireController.hpp
    30	    src/app/FireWidget.cpp
    31	    src/app/FireWidget.hpp
    32	    src/app/MainWindow.cpp
    33	    src/app/MainWindow.hpp
    34	    src/main.cpp
    35	)
    36	
    37	target_include_directories(classic_fire PRIVATE src)
    38	
    39	target_link_libraries(classic_fire PRIVATE
    40	    fire_simulation
    41	    Qt${QT_VERSION_MAJOR}::Widgets
    42	)
    43	fire_enable_sanitizers(classic_fire)
    44	
    45	set_target_properties(classic_fire PROPERTIES
    46	    AUTOMOC TRUE
    47	    MACOSX_BUNDLE TRUE
    48	    MACOSX_BUNDLE_BUNDLE_NAME "Classic Fire"
    49	    MACOSX_BUND
````

*[truncated, 1349 more characters]*

</details>

**🔧 Tool call — `Bash`**

*Read simulation library sources*

```bash
for f in src/sim/FireParameters.hpp src/sim/FireParameters.cpp src/sim/FireSimulation.hpp src/sim/FireSimulation.cpp; do echo "════════ $f ════════"; cat -n "$f"; done
```

<details>
<summary>Result</summary>

```
════════ src/sim/FireParameters.hpp ════════
     1	#pragma once
     2	
     3	#include <cstdint>
     4	
     5	// Tunable inputs to the fire simulation. Every setter clamps, so an instance is
     6	// always within range and can be handed to a UI layer as the authority on both
     7	// current values and permitted bounds.
     8	class FireParameters final {
     9	public:
    10	    static constexpr std::uint8_t MINIMUM_SOURCE_HEAT = 32;
    11	    static constexpr std::uint8_t MAXIMUM_SOURCE_HEAT = 255;
    12	    static constexpr std::uint8_t MINIMUM_COOLING = 1;
    13	    static constexpr std::uint8_t MAXIMUM_COOLING = 8;
    14	
    15	    void setSourceHeat(std::uint8_t sourceHeat) noexcept;
    16	    void setCooling(std::uint8_t cooling) noexcept;
    17	
    18	    [[nodiscard]] std::uint8_t sourceHeat() const noexcept { return sourceHeatLevel; }
    19	    [[nodiscard]] std::uint8_t cooling() const noexcept { return coolingRate; }
    20	
    21	private:
    22	    std::uint8_t sourceHeatLevel{MAXIMUM_SOURCE_HEAT};
    23	    std::uint8_t coolingRate{2};
    24	};
════════ src/sim/FireParameters.cpp ════════
     1	#include "sim/FireParameters.hpp"
     2	
     3	#include <algorithm>
     4	
     5	void FireParameters::setSourceHeat(const std::uint8_t sourceHeat) noexcept {
     6	    sourceHeatLevel = std::clamp(sourceHeat, MINIMUM_SOURCE_HEAT, MAXIMUM_SOURCE_HEAT);
     7	}
     8	
     9	void FireParameters::setCooling(const std::uint8_t cooling) noexcept {
    10	    coolingRate = std::clamp(cooling, MINIMUM_COOLING, MAXIMUM_COOLING);
    11	}
════════ src/sim/FireSimulation.hpp ════════
     1	#pragma once
     2	
     3	#include "sim/FireParameters.hpp"
     4	
     5	#include <cstddef>
     6	#include <cstdint>
     7	#include <span>
     8	#include <vector>
     9	
    10	class FireSimulation final {
    11	public:
    12	    explicit FireSimulation(std::size_t width, std::size_t height, std::uint32_t randomSeed = 0xC001CAFEu);
    13	
    14	    void tick() noexcept;
    15	    void reset() noexcept;
    16	
    17	    [[nodiscard]] std::size_t width() const noexcept { return simulationWidth; }
    18	    [[nodiscard]] std::size_t height() const noexcept { return simulationHeight; }
    19	    [[nodiscard]] std::span<const std::uint8_t> heat() const noexcept { return heatMap; }
    20	
    21	    // Parameters clamp themselves, so exposing them mutably cannot invalidate the
    22	    // simulation: tick() re-reads them each frame and holds no derived state.
    23	    [[nodiscard]] FireParameters& parameters() noexcept { return simulationParameters; }
    24	    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulationParameters; }
    25	
    26	private:
    27	    [[nodiscard]] std::uint32_t nextRandom() noexcept;
    28	    void updateFuelRow() noexcept;
    29	
    30	    std::size_t simulationWidth;
    31	    std::size_t simulationHeight;
    32	    std::vector<std::uint8_t> heatMap;
    33	    std::uint32_t initialSeed;
    34	    std::uint32_t randomState;
    35	    FireParameters simulationParameters;
    36	};
════════ src/sim/FireSimulation.cpp ════════
     1	#include "sim/FireSimulation.hpp"
     2	
     3	#include <algorithm>
     4	#include <limits>
     5	#include <stdexcept>
     6	
     7	namespace {
     8	constexpr std::uint32_t FALLBACK_SEED = 0xA341316Cu;
     9	constexpr std::uint32_t SIDE_WEIGHT = 1;
    10	constexpr std::uint32_t NEAR_CENTER_WEIGHT = 10;
    11	constexpr std::uint32_t FAR_CENTER_WEIGHT = 4;
    12	constexpr std::uint32_t WEIGHT_DIVISOR = 16;
    13	
    14	static_assert(2 * SIDE_WEIGHT + NEAR_CENTER_WEIGHT + FAR_CENTER_WEIGHT == WEIGHT_DIVISOR);
    15	} // namespace
    16	
    17	FireSimulation::FireSimulation(const std::size_t width, const std::size_t height, const std::uint32_t randomSeed)
    18	    : simulationWidth(width), simulationHeight(height), initialSeed(randomSeed == 0 ? FALLBACK_SEED : randomSeed),
    19	      randomState(initialSeed) {
    20	    if (simulationWidth < 2 || simulationHeight < 2) {
    21	        throw std::invalid_argument("Fire dimensions must both be at least 2");
    22	    }
    23	    if (simulationWidth > static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) ||
    24	        simulationHeight > std::numeric_limits<std::size_t>::max() / simulationWidth) {
    25	        throw std::length_error("Fire dimensions are too large");
    26	    }
    27	
    28	    heatMap.resize(simulationWidth * simulationHeight);
    29	    reset();
    30	}
    31	
    32	void FireSimulation::tick() noexcept {
    33	    auto* const cells = heatMap.data();
    34	    const std::uint8_t cooling = simulationParameters.cooling();
    35	
    36	    // Propagate from lower rows into the row above. Updating top-to-bottom is
    37	    // intentional: every source row still contains the previous frame's heat.
    38	    for (std::size_t y = 0; y + 1 < simulationHeight; ++y) {
    39	        const std::size_t destinationOffset = y * simulationWidth;
    40	        const std::size_t nearSourceOffset = destinationOffset + simulationWidth;
    41	        const std::size_t farSourceY = std::min(y + 2, simulationHeight - 1);
    42	        const std::size_t farSourceOffset = farSourceY * simulationWidth;
    43	
    44	        for (std::size_t x = 0; x < simulationWidth; ++x) {
    45	            const std::size_t leftX = x == 0 ? x : x - 1;
    46	            const std::size_t rightX = x + 1 == simulationWidth ? x : x + 1;
    47	            const std::uint32_t weightedHeat =
    48	                SIDE_WEIGHT * cells[nearSourceOffset + leftX] + NEAR_CENTER_WEIGHT * cells[nearSourceOffset + x] +
    49	                SIDE_WEIGHT * cells[nearSourceOffset + rightX] + FAR_CENTER_WEIGHT * cells[farSourceOffset + x];
    50	            const auto averagedHeat = static_cast<std::uint8_t>(weightedHeat / WEIGHT_DIVISOR);
    51	
    52	            cells[destinationOffset + x] =
    53	                averagedHeat > cooling ? static_cast<std::uint8_t>(averagedHeat - cooling) : std::uint8_t{0};
    54	        }
    55	    }
    56	
    57	    updateFuelRow();
    58	}
    59	
    60	void FireSimulation::reset() noexcept {
    61	    std::fill(heatMap.begin(), heatMap.end(), std::uint8_t{0});
    62	    randomState = initialSeed;
    63	    updateFuelRow();
    64	}
    65	
    66	std::uint32_t FireSimulation::nextRandom() noexcept {
    67	    // Xorshift32 is compact, deterministic and adequate for visual noise.
    68	    randomState ^= randomState << 13u;
    69	    randomState ^= randomState >> 17u;
    70	    randomState ^= randomState << 5u;
    71	    return randomState;
    72	}
    73	
    74	void FireSimulation::updateFuelRow() noexcept {
    75	    const std::size_t rowOffset = (simulationHeight - 1) * simulationWidth;
    76	    const std::uint8_t sourceHeat = simulationParameters.sourceHeat();
    77	    for (std::size_t x = 0; x < simulationWidth; ++x) {
    78	        const auto flicker = static_cast<std::uint8_t>(nextRandom() & 0x3Fu);
    79	        const auto scaledFlicker = static_cast<std::uint16_t>(flicker) * sourceHeat / 255u;
    80	        heatMap[rowOffset + x] = static_cast<std::uint8_t>(sourceHeat - scaledFlicker);
    81	    }
    82	}
```

</details>

**🔧 Tool call — `Bash`**

*Read MainWindow and FireController*

```bash
for f in src/app/MainWindow.hpp src/app/MainWindow.cpp src/app/FireController.hpp src/app/FireController.cpp; do echo "════════ $f ════════"; cat -n "$f"; done
```

<details>
<summary>Result</summary>

```
════════ src/app/MainWindow.hpp ════════
     1	#pragma once
     2	
     3	#include <QMainWindow>
     4	
     5	class MainWindow final : public QMainWindow {
     6	public:
     7	    explicit MainWindow(QWidget* parent = nullptr);
     8	
     9	private:
    10	    static constexpr int SIMULATION_WIDTH = 800;
    11	    static constexpr int SIMULATION_HEIGHT = 600;
    12	};
════════ src/app/MainWindow.cpp ════════
     1	#include "app/MainWindow.hpp"
     2	
     3	#include "app/ControlPanel.hpp"
     4	#include "app/FireController.hpp"
     5	#include "app/FireWidget.hpp"
     6	
     7	#include <QKeySequence>
     8	#include <QShortcut>
     9	#include <QVBoxLayout>
    10	#include <QWidget>
    11	
    12	MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
    13	    setWindowTitle(QStringLiteral("Fire Demo"));
    14	
    15	    auto* const centralWidget = new QWidget(this);
    16	    auto* const windowLayout = new QVBoxLayout(centralWidget);
    17	    windowLayout->setContentsMargins(0, 0, 0, 0);
    18	    windowLayout->setSpacing(0);
    19	
    20	    // TODO: remove duplicate constants
    21	    auto* const fireController = new FireController(SIMULATION_WIDTH, SIMULATION_HEIGHT, this);
    22	    auto* const fireWidget = new FireWidget(SIMULATION_WIDTH, SIMULATION_HEIGHT, centralWidget);
    23	    windowLayout->addWidget(fireWidget, 1);
    24	
    25	    auto* const controlPanel = new ControlPanel(fireController->parameters(), centralWidget);
    26	    windowLayout->addWidget(controlPanel);
    27	    setCentralWidget(centralWidget);
    28	
    29	    connect(controlPanel, &ControlPanel::toggleRequested, fireController, &FireController::toggleRunning);
    30	    connect(controlPanel, &ControlPanel::resetRequested, fireController, &FireController::reset);
    31	    connect(controlPanel, &ControlPanel::sourceHeatChanged, fireController, &FireController::setSourceHeat);
    32	    connect(controlPanel, &ControlPanel::coolingChanged, fireController, &FireController::setCooling);
    33	    const auto presentLatestFrame = [fireController, fireWidget] { fireWidget->present(fireController->heat()); };
    34	    connect(fireController, &FireController::frameReady, fireWidget, presentLatestFrame);
    35	    connect(fireController, &FireController::runningChanged, controlPanel, [controlPanel](const bool running) {
    36	        controlPanel->setPaused(!running);
    37	    });
    38	
    39	    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    40	    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    41	    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    42	    connect(pauseShortcut, &QShortcut::activated, fireController, &FireController::toggleRunning);
    43	    connect(resetShortcut, &QShortcut::activated, fireController, &FireController::reset);
    44	    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);
    45	
    46	    presentLatestFrame();
    47	    fireController->run();
    48	    resize(960, 720);
    49	}
════════ src/app/FireController.hpp ════════
     1	#pragma once
     2	
     3	#include "sim/FireSimulation.hpp"
     4	
     5	#include <QObject>
     6	#include <QTimer>
     7	
     8	#include <chrono>
     9	#include <cstddef>
    10	#include <cstdint>
    11	#include <span>
    12	
    13	class FireController final : public QObject {
    14	    Q_OBJECT
    15	
    16	public:
    17	    explicit FireController(std::size_t simulationWidth, std::size_t simulationHeight, QObject* parent = nullptr);
    18	
    19	    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    20	    [[nodiscard]] std::span<const std::uint8_t> heat() const noexcept { return simulation.heat(); }
    21	    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }
    22	
    23	public slots:
    24	    void run();
    25	    void pause();
    26	    void toggleRunning();
    27	    void reset();
    28	    void setSourceHeat(int sourceHeat);
    29	    void setCooling(int cooling);
    30	
    31	signals:
    32	    void frameReady();
    33	    void runningChanged(bool running);
    34	
    35	private slots:
    36	    void advanceFrame();
    37	
    38	private:
    39	    using Clock = std::chrono::steady_clock;
    40	
    41	    static constexpr int WAKE_INTERVAL_MILLISECONDS = 16;
    42	    static constexpr int SIMULATION_TICKS_PER_SECOND = 60;
    43	    static constexpr int MAX_TICKS_PER_WAKE = 3;
    44	    static constexpr std::chrono::duration<double> SIMULATION_STEP{1.0 / SIMULATION_TICKS_PER_SECOND};
    45	
    46	    FireSimulation simulation;
    47	    QTimer wakeTimer;
    48	    Clock::time_point elapsedTimeReference;
    49	    std::chrono::duration<double> accumulatedTime{0.0};
    50	};
════════ src/app/FireController.cpp ════════
     1	#include "app/FireController.hpp"
     2	
     3	FireController::FireController(const std::size_t simulationWidth,
     4	                               const std::size_t simulationHeight,
     5	                               QObject* const parent)
     6	    : QObject(parent), simulation(simulationWidth, simulationHeight) {
     7	    wakeTimer.setTimerType(Qt::PreciseTimer);
     8	    wakeTimer.setInterval(WAKE_INTERVAL_MILLISECONDS);
     9	    connect(&wakeTimer, &QTimer::timeout, this, &FireController::advanceFrame);
    10	}
    11	
    12	void FireController::run() {
    13	    if (isRunning()) {
    14	        return;
    15	    }
    16	
    17	    elapsedTimeReference = Clock::now();
    18	    wakeTimer.start();
    19	    emit runningChanged(true);
    20	}
    21	
    22	void FireController::pause() {
    23	    if (!isRunning()) {
    24	        return;
    25	    }
    26	
    27	    wakeTimer.stop();
    28	    emit runningChanged(false);
    29	}
    30	
    31	void FireController::toggleRunning() {
    32	    if (isRunning()) {
    33	        pause();
    34	    } else {
    35	        run();
    36	    }
    37	}
    38	
    39	void FireController::reset() {
    40	    simulation.reset();
    41	    emit frameReady();
    42	}
    43	
    44	void FireController::setSourceHeat(const int sourceHeat) {
    45	    simulation.parameters().setSourceHeat(static_cast<std::uint8_t>(sourceHeat));
    46	}
    47	
    48	void FireController::setCooling(const int cooling) {
    49	    simulation.parameters().setCooling(static_cast<std::uint8_t>(cooling));
    50	}
    51	
    52	void FireController::advanceFrame() {
    53	    const auto now = Clock::now();
    54	    accumulatedTime += now - elapsedTimeReference;
    55	    elapsedTimeReference = now;
    56	
    57	    const auto maximumCatchUpTime = SIMULATION_STEP * MAX_TICKS_PER_WAKE;
    58	    if (accumulatedTime > maximumCatchUpTime) {
    59	        accumulatedTime = maximumCatchUpTime;
    60	    }
    61	
    62	    int ticksExecuted = 0;
    63	    while (accumulatedTime >= SIMULATION_STEP && ticksExecuted < MAX_TICKS_PER_WAKE) {
    64	        simulation.tick();
    65	        accumulatedTime -= SIMULATION_STEP;
    66	        ++ticksExecuted;
    67	    }
    68	
    69	    if (ticksExecuted > 0) {
    70	        emit frameReady();
    71	    }
    72	}
```

</details>

**🔧 Tool call — `Bash`**

*Read FireWidget and ControlPanel*

```bash
for f in src/app/FireWidget.hpp src/app/FireWidget.cpp src/app/ControlPanel.hpp src/app/ControlPanel.cpp; do echo "════════ $f ════════"; cat -n "$f"; done
```

<details>
<summary>Result</summary>

```
════════ src/app/FireWidget.hpp ════════
     1	#pragma once
     2	
     3	#include <QImage>
     4	#include <QWidget>
     5	
     6	#include <array>
     7	#include <cstdint>
     8	#include <span>
     9	
    10	class FireWidget final : public QWidget {
    11	public:
    12	    explicit FireWidget(int simulationWidth, int simulationHeight, QWidget* parent = nullptr);
    13	
    14	    void present(std::span<const std::uint8_t> heat) noexcept;
    15	
    16	    [[nodiscard]] QSize minimumSizeHint() const override;
    17	    [[nodiscard]] QSize sizeHint() const override;
    18	
    19	protected:
    20	    void paintEvent(QPaintEvent* event) override;
    21	
    22	private:
    23	    [[nodiscard]] static std::array<QRgb, 256> makePalette() noexcept;
    24	    [[nodiscard]] QRect fittedFrameRect() const noexcept;
    25	
    26	    QImage frame;
    27	    std::array<QRgb, 256> palette;
    28	};
════════ src/app/FireWidget.cpp ════════
     1	#include "app/FireWidget.hpp"
     2	
     3	#include <QPaintEvent>
     4	#include <QPainter>
     5	
     6	#include <array>
     7	#include <cassert>
     8	
     9	namespace {
    10	struct PaletteStop {
    11	    int index;
    12	    int red;
    13	    int green;
    14	    int blue;
    15	};
    16	
    17	constexpr std::array PALETTE_STOPS{
    18	    PaletteStop{0, 0, 0, 0},
    19	    PaletteStop{32, 45, 0, 0},
    20	    PaletteStop{80, 150, 0, 0},
    21	    PaletteStop{136, 255, 45, 0},
    22	    PaletteStop{192, 255, 160, 0},
    23	    PaletteStop{232, 255, 245, 80},
    24	    PaletteStop{255, 255, 255, 255},
    25	};
    26	} // namespace
    27	
    28	FireWidget::FireWidget(const int simulationWidth, const int simulationHeight, QWidget* const parent)
    29	    : QWidget(parent), frame(simulationWidth, simulationHeight, QImage::Format_RGB32), palette(makePalette()) {
    30	    assert(!frame.isNull());
    31	    setAttribute(Qt::WA_OpaquePaintEvent);
    32	    setAutoFillBackground(false);
    33	    setFocusPolicy(Qt::StrongFocus);
    34	    frame.fill(Qt::black);
    35	}
    36	
    37	void FireWidget::present(const std::span<const std::uint8_t> heat) noexcept {
    38	    const auto expectedSize = static_cast<std::size_t>(frame.width()) * static_cast<std::size_t>(frame.height());
    39	    assert(heat.size() == expectedSize);
    40	    if (heat.size() != expectedSize) {
    41	        return;
    42	    }
    43	
    44	    std::size_t sourceOffset = 0;
    45	    for (int y = 0; y < frame.height(); ++y) {
    46	        auto* const destination = reinterpret_cast<QRgb*>(frame.scanLine(y));
    47	        for (int x = 0; x < frame.width(); ++x) {
    48	            destination[x] = palette[heat[sourceOffset++]];
    49	        }
    50	    }
    51	
    52	    update();
    53	}
    54	
    55	QSize FireWidget::minimumSizeHint() const { return {400, 300}; }
    56	
    57	QSize FireWidget::sizeHint() const { return {800, 600}; }
    58	
    59	void FireWidget::paintEvent(QPaintEvent* const event) {
    60	    Q_UNUSED(event);
    61	
    62	    QPainter painter(this);
    63	    painter.fillRect(rect(), Qt::black);
    64	    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    65	    painter.drawImage(fittedFrameRect(), frame);
    66	}
    67	
    68	std::array<QRgb, 256> FireWidget::makePalette() noexcept {
    69	    std::array<QRgb, 256> palette{};
    70	
    71	    for (std::size_t segment = 0; segment + 1 < PALETTE_STOPS.size(); ++segment) {
    72	        const PaletteStop& first = PALETTE_STOPS[segment];
    73	        const PaletteStop& second = PALETTE_STOPS[segment + 1];
    74	        const int distance = second.index - first.index;
    75	
    76	        for (int index = first.index; index <= second.index; ++index) {
    77	            const int offset = index - first.index;
    78	            const int red = first.red + (second.red - first.red) * offset / distance;
    79	            const int green = first.green + (second.green - first.green) * offset / distance;
    80	            const int blue = first.blue + (second.blue - first.blue) * offset / distance;
    81	            palette[static_cast<std::size_t>(index)] = qRgb(red, green, blue);
    82	        }
    83	    }
    84	
    85	    return palette;
    86	}
    87	
    88	QRect FireWidget::fittedFrameRect() const noexcept {
    89	    const int availableWidth = width();
    90	    const int availableHeight = height();
    91	    if (availableWidth <= 0 || availableHeight <= 0) {
    92	        return {};
    93	    }
    94	
    95	    const qint64 widthLimitedHeight = static_cast<qint64>(availableWidth) * frame.height();
    96	    const qint64 heightLimitedWidth = static_cast<qint64>(availableHeight) * frame.width();
    97	
    98	    int renderedWidth = availableWidth;
    99	    int renderedHeight = availableHeight;
   100	    if (widthLimitedHeight <= heightLimitedWidth) {
   101	        renderedHeight = static_cast<int>(widthLimitedHeight / frame.width());
   102	    } else {
   103	        renderedWidth = static_cast<int>(heightLimitedWidth / frame.height());
   104	    }
   105	
   106	    return {
   107	        (availableWidth - renderedWidth) / 2, (availableHeight - renderedHeight) / 2, renderedWidth, renderedHeight};
   108	}
════════ src/app/ControlPanel.hpp ════════
     1	#pragma once
     2	
     3	#include <QWidget>
     4	
     5	class FireParameters;
     6	class QPushButton;
     7	
     8	class ControlPanel final : public QWidget {
     9	    Q_OBJECT
    10	
    11	public:
    12	    explicit ControlPanel(const FireParameters& parameters, QWidget* parent = nullptr);
    13	
    14	    void setPaused(bool paused);
    15	
    16	signals:
    17	    void toggleRequested();
    18	    void resetRequested();
    19	    void sourceHeatChanged(int sourceHeat);
    20	    void coolingChanged(int cooling);
    21	
    22	private:
    23	    QPushButton* pauseButton{nullptr};
    24	};
════════ src/app/ControlPanel.cpp ════════
     1	#include "app/ControlPanel.hpp"
     2	
     3	#include "sim/FireParameters.hpp"
     4	
     5	#include <QHBoxLayout>
     6	#include <QLabel>
     7	#include <QPushButton>
     8	#include <QSlider>
     9	
    10	namespace {
    11	QLabel* makeValueLabel(QWidget* const parent, const int initialValue) {
    12	    auto* const label = new QLabel(QString::number(initialValue), parent);
    13	    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    14	    label->setMinimumWidth(28);
    15	    return label;
    16	}
    17	
    18	QSlider* addSlider(QHBoxLayout& layout,
    19	                   QWidget* const parent,
    20	                   const QString& title,
    21	                   const int minimum,
    22	                   const int maximum,
    23	                   const int initialValue) {
    24	    auto* const titleLabel = new QLabel(title, parent);
    25	    auto* const slider = new QSlider(Qt::Horizontal, parent);
    26	    auto* const valueLabel = makeValueLabel(parent, initialValue);
    27	    slider->setRange(minimum, maximum);
    28	    slider->setValue(initialValue);
    29	    slider->setMinimumWidth(90);
    30	
    31	    QObject::connect(
    32	        slider, &QSlider::valueChanged, valueLabel, [valueLabel](const int value) { valueLabel->setNum(value); });
    33	
    34	    layout.addWidget(titleLabel);
    35	    layout.addWidget(slider, 1);
    36	    layout.addWidget(valueLabel);
    37	    return slider;
    38	}
    39	} // namespace
    40	
    41	ControlPanel::ControlPanel(const FireParameters& parameters, QWidget* const parent) : QWidget(parent) {
    42	    auto* const controlsLayout = new QHBoxLayout(this);
    43	    controlsLayout->setContentsMargins(12, 8, 12, 8);
    44	
    45	    pauseButton = new QPushButton(QStringLiteral("Pause"), this);
    46	    auto* const resetButton = new QPushButton(QStringLiteral("Reset"), this);
    47	    pauseButton->setToolTip(QStringLiteral("Pause o
```

*[truncated, 1758 more characters]*

</details>

**🔧 Tool call — `Bash`**

*Check history for metrics library*

```bash
echo "=== git log all ==="; git log --oneline --all | head -30; echo "=== fire_metrics in history ==="; git log --all --oneline -S 'fire_metrics' -- . | head; echo "=== metrics grep in tree ==="; grep -ril 'metric\|profil\|monitor\|instrument' --include='*.cpp' --include='*.hpp' --include='*.txt' --include='*.md' --include='*.cmake' --include='*.json' . 2>/dev/null | grep -v '^./build' ; echo "=== build.ninja metrics refs ==="; grep -o 'fire_metrics[^ ]*' build/release/build.ninja | sort -u | head
```

<details>
<summary>Result</summary>

```
=== git log all ===
24fa3eb Add build configuration with sanitizers enabled
92f03d7 Add initial version of performance metrics and panel to display them
122e046 Brake down into smaller types
20e9371 Add build configuration with sanitizers enabled
ea18d3d Add initial version of performance metrics and panel to display them
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
=== fire_metrics in history ===
122e046 Brake down into smaller types
=== metrics grep in tree ===
CMakePresets.json
README.md
=== build.ninja metrics refs ===
```

</details>

**🔧 Tool call — `Bash`**

*Inspect metrics branches*

```bash
echo "=== branches ==="; git branch -a -v; echo; echo "=== diffstat: main..metrics commit ea18d3d ==="; git diff --stat 0ef7d02 ea18d3d 2>/dev/null; echo; echo "=== files changed ea18d3d ==="; git show --stat --oneline ea18d3d | head -30; echo; echo "=== 122e046 stat ==="; git show --stat --oneline 122e046 | head -40
```

<details>
<summary>Result</summary>

```
=== branches ===
* main                            24fa3eb Add build configuration with sanitizers enabled
  perf-monitor-cdx                92f03d7 Add initial version of performance metrics and panel to display them
  refactor-composition            122e046 Brake down into smaller types
  remotes/origin/main             24fa3eb Add build configuration with sanitizers enabled
  remotes/origin/perf-monitor-cdx 92f03d7 Add initial version of performance metrics and panel to display them

=== diffstat: main..metrics commit ea18d3d ===
 CMakeLists.txt                 |   4 +
 src/app/FireController.cpp     |  10 ++-
 src/app/FireController.hpp     |   8 +-
 src/app/FireWidget.cpp         |  24 ++++--
 src/app/FireWidget.hpp         |   8 +-
 src/app/MainWindow.cpp         |  10 ++-
 src/app/MainWindow.hpp         |   4 +
 src/app/PerformanceMonitor.cpp |  95 +++++++++++++++++++++++
 src/app/PerformanceMonitor.hpp |  63 +++++++++++++++
 src/app/PerformancePanel.cpp   | 170 +++++++++++++++++++++++++++++++++++++++++
 src/app/PerformancePanel.hpp   |  30 ++++++++
 11 files changed, 415 insertions(+), 11 deletions(-)

=== files changed ea18d3d ===
ea18d3d Add initial version of performance metrics and panel to display them
 CMakeLists.txt                 |   4 +
 src/app/FireController.cpp     |  10 ++-
 src/app/FireController.hpp     |   8 +-
 src/app/FireWidget.cpp         |  24 ++++--
 src/app/FireWidget.hpp         |   8 +-
 src/app/MainWindow.cpp         |  10 ++-
 src/app/MainWindow.hpp         |   4 +
 src/app/PerformanceMonitor.cpp |  95 +++++++++++++++++++++++
 src/app/PerformanceMonitor.hpp |  63 +++++++++++++++
 src/app/PerformancePanel.cpp   | 170 +++++++++++++++++++++++++++++++++++++++++
 src/app/PerformancePanel.hpp   |  30 ++++++++
 11 files changed, 415 insertions(+), 11 deletions(-)

=== 122e046 stat ===
122e046 Brake down into smaller types
 CMakeLists.txt                 |  19 ++++++++
 src/app/FireController.cpp     |  22 ++++++---
 src/app/FireController.hpp     |   5 +-
 src/app/FirePalette.cpp        |  40 ++++++++++++++++
 src/app/FirePalette.hpp        |  23 +++++++++
 src/app/FireRenderer.cpp       |  38 +++++++++++++++
 src/app/FireRenderer.hpp       |  24 ++++++++++
 src/app/FireWidget.cpp         | 105 ++++++++++-------------------------------
 src/app/FireWidget.hpp         |  22 ++++-----
 src/app/MainWindow.cpp         |  26 +++++++---
 src/app/MainWindow.hpp         |  15 +++---
 src/app/PerformanceMonitor.cpp |  91 ++---------------------------------
 src/app/PerformanceMonitor.hpp |  63 ++++++-------------------
 src/app/PerformancePanel.cpp   |  12 ++---
 src/main.cpp                   |   7 ++-
 src/metrics/CounterMetric.hpp  |  15 ++++++
 src/metrics/FrameMetric.cpp    |  49 +++++++++++++++++++
 src/metrics/FrameMetric.hpp    |  38 +++++++++++++++
 src/metrics/ScopedMetric.hpp   |  21 +++++++++
 src/sim/FireSimulation.hpp     |   8 ++--
 src/sim/HeatFrame.hpp          |  34 +++++++++++++
 21 files changed, 416 insertions(+), 261 deletions(-)
```

</details>

**🔧 Tool call — `Bash`**

*Diff of metrics integration into existing files*

```bash
git diff 0ef7d02 ea18d3d -- src/app/FireController.hpp src/app/FireController.cpp src/app/MainWindow.hpp src/app/MainWindow.cpp src/app/FireWidget.hpp src/app/FireWidget.cpp CMakeLists.txt
```

<details>
<summary>Result</summary>

```
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 754f47d..5143581 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -28,6 +28,10 @@ add_executable(classic_fire
     src/app/FireWidget.hpp
     src/app/MainWindow.cpp
     src/app/MainWindow.hpp
+    src/app/PerformanceMonitor.cpp
+    src/app/PerformanceMonitor.hpp
+    src/app/PerformancePanel.cpp
+    src/app/PerformancePanel.hpp
     src/main.cpp
 )
 
diff --git a/src/app/FireController.cpp b/src/app/FireController.cpp
index 3249f1e..bb299ad 100644
--- a/src/app/FireController.cpp
+++ b/src/app/FireController.cpp
@@ -1,9 +1,12 @@
 #include "app/FireController.hpp"
 
+#include "app/PerformanceMonitor.hpp"
+
 FireController::FireController(const std::size_t simulationWidth,
                                const std::size_t simulationHeight,
+                               PerformanceMonitor& performanceMonitor,
                                QObject* const parent)
-    : QObject(parent), simulation(simulationWidth, simulationHeight) {
+    : QObject(parent), simulation(simulationWidth, simulationHeight), performanceMonitor(performanceMonitor) {
     wakeTimer.setTimerType(Qt::PreciseTimer);
     wakeTimer.setInterval(WAKE_INTERVAL_MILLISECONDS);
     connect(&wakeTimer, &QTimer::timeout, this, &FireController::advanceFrame);
@@ -14,6 +17,7 @@ void FireController::run() {
         return;
     }
 
+    performanceMonitor.clearMeasurements();
     elapsedTimeReference = Clock::now();
     wakeTimer.start();
     emit runningChanged(true);
@@ -37,6 +41,7 @@ void FireController::toggleRunning() {
 }
 
 void FireController::reset() {
+    performanceMonitor.reset();
     simulation.reset();
     emit frameReady();
 }
@@ -61,12 +66,15 @@ void FireController::advanceFrame() {
 
     int ticksExecuted = 0;
     while (accumulatedTime >= SIMULATION_STEP && ticksExecuted < MAX_TICKS_PER_WAKE) {
+        const auto tickStarted = Clock::now();
         simulation.tick();
+        performanceMonitor.recordSimulationTick(Clock::now() - tickStarted);
         accumulatedTime -= SIMULATION_STEP;
         ++ticksExecuted;
     }
 
     if (ticksExecuted > 0) {
+        performanceMonitor.recordSimulationFrames(ticksExecuted);
         emit frameReady();
     }
 }
diff --git a/src/app/FireController.hpp b/src/app/FireController.hpp
index 2fecaad..b85bfe5 100644
--- a/src/app/FireController.hpp
+++ b/src/app/FireController.hpp
@@ -10,11 +10,16 @@
 #include <cstdint>
 #include <span>
 
+class PerformanceMonitor;
+
 class FireController final : public QObject {
     Q_OBJECT
 
 public:
-    explicit FireController(std::size_t simulationWidth, std::size_t simulationHeight, QObject* parent = nullptr);
+    explicit FireController(std::size_t simulationWidth,
+                            std::size_t simulationHeight,
+                            PerformanceMonitor& performanceMonitor,
+                            QObject* parent = nullptr);
 
     [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
     [[nodiscard]] std::span<const std::uint8_t> heat() const noexcept { return simulation.heat(); }
@@ -44,6 +49,7 @@ private:
     static constexpr std::chrono::duration<double> SIMULATION_STEP{1.0 / SIMULATION_TICKS_PER_SECOND};
 
     FireSimulation simulation;
+    PerformanceMonitor& performanceMonitor;
     QTimer wakeTimer;
     Clock::time_point elapsedTimeReference;
     std::chrono::duration<double> accumulatedTime{0.0};
diff --git a/src/app/FireWidget.cpp b/src/app/FireWidget.cpp
index 3eb4e54..aac0960 100644
--- a/src/app/FireWidget.cpp
+++ b/src/app/FireWidget.cpp
@@ -1,5 +1,7 @@
 #include "app/FireWidget.hpp"
 
+#include "app/PerformanceMonitor.hpp"
+
 #include <QPaintEvent>
 #include <QPainter>
 
@@ -25,8 +27,12 @@ constexpr std::array PALETTE_STOPS{
 };
 } // namespace
 
-FireWidget::FireWidget(const int simulationWidth, const int simulationHeight, QWidget* const parent)
-    : QWidget(parent), frame(simulationWidth, simulationHeight, QImage::Format_RGB32), palette(makePalette()) {
+FireWidget::FireWidget(const int simulationWidth,
+                       const int simulationHeight,
+                       PerformanceMonitor& performanceMonitor,
+                       QWidget* const parent)
+    : QWidget(parent), frame(simulationWidth, simulationHeight, QImage::Format_RGB32), palette(makePalette()),
+      performanceMonitor(performanceMonitor) {
     assert(!frame.isNull());
     setAttribute(Qt::WA_OpaquePaintEvent);
     setAutoFillBackground(false);
@@ -41,6 +47,7 @@ void FireWidget::present(const std::span<const std::uint8_t> heat) noexcept {
         return;
     }
 
+    const auto conversionStarted = PerformanceMonitor::Clock::now();
     std::size_t sourceOffset = 0;
     for (int y = 0; y < frame.height(); ++y) {
         auto* const destination = reinterpret_cast<QRgb*>(frame.scanLine(y));
@@ -48,6 +55,7 @@ void FireWidget::present(const std::span<const std::uint8_t> heat) noexcept {
             destination[x] = palette[heat[sourceOffset++]];
         }
     }
+    performanceMonitor.recordHeatToImage(PerformanceMonitor::Clock::now() - conversionStarted);
 
     update();
 }
@@ -59,10 +67,14 @@ QSize FireWidget::sizeHint() const { return {800, 600}; }
 void FireWidget::paintEvent(QPaintEvent* const event) {
     Q_UNUSED(event);
 
-    QPainter painter(this);
-    painter.fillRect(rect(), Qt::black);
-    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
-    painter.drawImage(fittedFrameRect(), frame);
+    const auto paintStarted = PerformanceMonitor::Clock::now();
+    {
+        QPainter painter(this);
+        painter.fillRect(rect(), Qt::black);
+        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
+        painter.drawImage(fittedFrameRect(), frame);
+    }
+    performanceMonitor.recordPaintEvent(PerformanceMonitor::Clock::now() - paintStarted);
 }
 
 std::array<QRgb, 256> FireWidget::makePalette() noexcept {
diff --git a/src/app/FireWidget.hpp b/src/app/FireWidget.hpp
index 3021fe6..cdb3bae 100644
--- a/src/app/FireWidget.hpp
+++ b/src/app/FireWidget.hpp
@@ -7,9 +7,14 @@
 #include <cstdint>
 #include <span>
 
+class PerformanceMonitor;
+
 class FireWidget final : public QWidget {
 public:
-    explicit FireWidget(int simulationWidth, int simulationHeight, QWidget* parent = nullptr);
+    explicit FireWidget(int simulationWidth,
+                        int simulationHeight,
+                        PerformanceMonitor& performanceMonitor,
+                        QWidget* parent = nullptr);
 
     void present(std::span<const std::uint8_t> heat) noexcept;
 
@@ -25,4 +30,5 @@ private:
 
     QImage frame;
     std::array<QRgb, 256> palette;
+    PerformanceMonitor& performanceMonitor;
 };
diff --git a/src/app/MainWindow.cpp b/src/app/MainWindow.cpp
index 8341dfe..a246ea2 100644
--- a/src/app/MainWindow.cpp
+++ b/src/app/MainWindow.cpp
@@ -3,6 +3,7 @@
 #include "app/ControlPanel.hpp"
 #include "app/FireController.hpp"
 #include "app/FireWidget.hpp"
+#include "app/PerformancePanel.hpp"
 
 #include <QKeySequence>
 #include <QShortcut>
@@ -18,12 +19,14 @@ MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
     windowLayout->setSpacing(0);
 
     // TODO: remove duplicate constants
-    auto* const fireController = new FireController(SIMULATION_WIDTH, SIMULATION_HEIGHT, this);
-    auto* const fireWidget = new FireWidget(SIMULATION_WIDTH, SIMULATION_HEIGHT, centralWidget);
+    auto* const fireController = new FireController(SIMULATION_WIDTH, SIMULATION_HEIGHT, performanceMonitor, this);
+    auto* const fireWidget = new FireWidget(SIMULATION_WIDTH, SIMULATION_HEIGHT, performanceMonitor, centralWidget);
     windowLayout->addWidget(fireWidget, 1);
 
     auto* const controlPanel = new ControlPanel(fireController->parameters(), centralWidget);
     windowLayout->addWidget(controlPanel);
+    auto* const performancePanel = new PerformancePanel(performanceMonitor, centralWidget);
+    windowLayout->addWidget(performance
```

*[truncated, 1232 more characters]*

</details>

**🔧 Tool call — `Bash`**

*Inspect refactor-composition branch design*

```bash
for f in src/metrics/FrameMetric.hpp src/metrics/ScopedMetric.hpp src/metrics/CounterMetric.hpp src/sim/HeatFrame.hpp src/app/FireRenderer.hpp src/app/PerformanceMonitor.hpp; do echo "════════ $f ════════"; git show 122e046:$f 2>/dev/null || echo "(absent)"; done; echo "════ CMake diff ════"; git diff ea18d3d 122e046 -- CMakeLists.txt
```

<details>
<summary>Result</summary>

```
════════ src/metrics/FrameMetric.hpp ════════
#pragma once

#include <array>
#include <chrono>
#include <cstddef>

// Summary of one timing channel over the recent sampling window.
struct MetricStatistics {
    double averageMilliseconds{0.0};
    double percentile95Milliseconds{0.0};
    std::size_t sampleCount{0};
};

// A rolling window of durations. It knows nothing about what it measures:
// callers name a channel by choosing which instance to write to.
class FrameMetric final {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = Clock::duration;

    void record(Duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics(Clock::time_point now) const noexcept;

private:
    static constexpr std::size_t MAXIMUM_SAMPLE_COUNT = 512;
    static constexpr auto STATISTICS_WINDOW = std::chrono::seconds{2};

    struct TimedSample {
        Clock::time_point recordedAt;
        Duration duration;
    };

    std::array<TimedSample, MAXIMUM_SAMPLE_COUNT> samples{};
    std::size_t nextSample{0};
    std::size_t sampleCount{0};
};
════════ src/metrics/ScopedMetric.hpp ════════
#pragma once

#include "metrics/FrameMetric.hpp"

// Times the enclosing scope and records the result on destruction, replacing the
// manual now()/subtract/record sequence at every measured site. Declaring one
// before other scoped resources measures their teardown too, because they are
// destroyed first.
class ScopedMetric final {
public:
    explicit ScopedMetric(FrameMetric& metric) noexcept : metric(metric), startedAt(FrameMetric::Clock::now()) {}

    ScopedMetric(const ScopedMetric&) = delete;
    ScopedMetric& operator=(const ScopedMetric&) = delete;

    ~ScopedMetric() noexcept { metric.record(FrameMetric::Clock::now() - startedAt); }

private:
    FrameMetric& metric;
    FrameMetric::Clock::time_point startedAt;
};
════════ src/metrics/CounterMetric.hpp ════════
#pragma once

#include <cstdint>

// A monotonic tally. What is being counted, and why, belongs to the caller.
class CounterMetric final {
public:
    void add(const std::uint64_t amount = 1) noexcept { total += amount; }
    void clear() noexcept { total = 0; }

    [[nodiscard]] std::uint64_t value() const noexcept { return total; }

private:
    std::uint64_t total{0};
};
════════ src/sim/HeatFrame.hpp ════════
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

// Non-owning view of one simulation frame: the heat buffer together with the
// geometry that gives it meaning. Constructing one establishes
// cells.size() == width * height, so every consumer receives a correctly shaped
// buffer by construction and none of them has to re-check the agreement.
class HeatFrame final {
public:
    constexpr HeatFrame(const std::span<const std::uint8_t> cells,
                        const std::size_t width,
                        const std::size_t height) noexcept
        : heatCells(cells), frameWidth(width), frameHeight(height) {
        assert(cells.size() == width * height);
    }

    [[nodiscard]] constexpr std::size_t width() const noexcept { return frameWidth; }
    [[nodiscard]] constexpr std::size_t height() const noexcept { return frameHeight; }
    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept { return heatCells; }

    [[nodiscard]] constexpr std::span<const std::uint8_t> row(const std::size_t y) const noexcept {
        assert(y < frameHeight);
        return heatCells.subspan(y * frameWidth, frameWidth);
    }

private:
    std::span<const std::uint8_t> heatCells;
    std::size_t frameWidth;
    std::size_t frameHeight;
};
════════ src/app/FireRenderer.hpp ════════
#pragma once

#include "app/FirePalette.hpp"

#include <QImage>

class HeatFrame;

// Converts a simulation frame into a displayable image. Owns the destination
// image and sizes it from the frame it is handed, so frame geometry is never
// configured separately from the data it describes.
class FireRenderer final {
public:
    explicit FireRenderer(const FirePalette& palette) noexcept : palette(palette) {}

    // Reallocates the destination image when the frame geometry changes.
    void render(const HeatFrame& frame);

    [[nodiscard]] const QImage& image() const noexcept { return frameImage; }

private:
    FirePalette palette;
    QImage frameImage;
};
════════ src/app/PerformanceMonitor.hpp ════════
#pragma once

#include "metrics/CounterMetric.hpp"
#include "metrics/FrameMetric.hpp"

#include <cstdint>

// The set of channels this application measures. This is the only place that
// names the pipeline stages; the metric types themselves are stage-agnostic, and
// each channel decides for itself what recording means. Producers take the
// monitor by mutable reference and readers by const reference, which is what
// keeps the read side out of the write side.
class PerformanceMonitor final {
public:
    struct Snapshot {
        MetricStatistics simulationTick;
        MetricStatistics heatToImage;
        MetricStatistics paintEvent;
        std::uint64_t catchUpTicks{0};
        std::uint64_t totalTicks{0};
    };

    [[nodiscard]] Snapshot snapshot() const noexcept;

    FrameMetric simulationTick;
    FrameMetric heatToImage;
    FrameMetric paintEvent;
    CounterMetric totalTicks;
    CounterMetric catchUpTicks;
};
════ CMake diff ════
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 5143581..a2449ef 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -6,6 +6,8 @@ set(CMAKE_CXX_STANDARD 20)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)
 set(CMAKE_CXX_EXTENSIONS OFF)
 
+include(cmake/Sanitizers.cmake)
+
 find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
 find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
 
@@ -14,16 +16,33 @@ add_library(fire_simulation
     src/sim/FireParameters.hpp
     src/sim/FireSimulation.cpp
     src/sim/FireSimulation.hpp
+    src/sim/HeatFrame.hpp
 )
 
 target_include_directories(fire_simulation PUBLIC src)
 target_compile_features(fire_simulation PUBLIC cxx_std_20)
+fire_enable_sanitizers(fire_simulation)
+
+add_library(fire_metrics
+    src/metrics/CounterMetric.hpp
+    src/metrics/FrameMetric.cpp
+    src/metrics/FrameMetric.hpp
+    src/metrics/ScopedMetric.hpp
+)
+
+target_include_directories(fire_metrics PUBLIC src)
+target_compile_features(fire_metrics PUBLIC cxx_std_20)
+fire_enable_sanitizers(fire_metrics)
 
 add_executable(classic_fire
     src/app/ControlPanel.cpp
     src/app/ControlPanel.hpp
     src/app/FireController.cpp
     src/app/FireController.hpp
+    src/app/FirePalette.cpp
+    src/app/FirePalette.hpp
+    src/app/FireRenderer.cpp
+    src/app/FireRenderer.hpp
     src/app/FireWidget.cpp
     src/app/FireWidget.hpp
     src/app/MainWindow.cpp
@@ -38,9 +57,11 @@ add_executable(classic_fire
 target_include_directories(classic_fire PRIVATE src)
 
 target_link_libraries(classic_fire PRIVATE
+    fire_metrics
     fire_simulation
     Qt${QT_VERSION_MAJOR}::Widgets
 )
+fire_enable_sanitizers(classic_fire)
 
 set_target_properties(classic_fire PROPERTIES
     AUTOMOC TRUE
@@ -60,8 +81,10 @@ add_custom_target(run_app
 
 if(MSVC)
     target_compile_options(fire_simulation PRIVATE /W4 /permissive-)
+    target_compile_options(fire_metrics PRIVATE /W4 /permissive-)
     target_compile_options(classic_fire PRIVATE /W4 /permissive-)
 else()
     target_compile_options(fire_simulation PRIVATE -Wall -Wextra -Wpedantic)
+    target_compile_options(fire_metrics PRIVATE -Wall -Wextra -Wpedantic)
     target_compile_options(classic_fire PRIVATE -Wall -Wextra -Wpedantic)
 endif()
```

</details>

**🔧 Tool call — `Bash`**

*Read build tooling and fps meter removal*

```bash
echo "════ justfile ════"; cat justfile; echo "════ presets ════"; cat CMakePresets.json; echo "════ Sanitizers.cmake ════"; cat cmake/Sanitizers.cmake; echo "════ removed fps meter (34d7ffa) ════"; git show --stat --oneline 34d7ffa
```

<details>
<summary>Result</summary>

```
════ justfile ════
default: run-release

configure-debug:
    cmake --fresh --preset cfg-debug

configure-release:
    cmake --fresh --preset cfg-release

configure-profile:
    cmake --fresh --preset cfg-profile

configure-sanitize:
    cmake --fresh --preset cfg-sanitize

build-debug: configure-debug
    cmake --build --preset build-debug

build-release: configure-release
    cmake --build --preset build-release

build-profile: configure-profile
    cmake --build --preset build-profile

build-sanitize: configure-sanitize
    cmake --build --preset build-sanitize

build-all: build-debug build-release build-profile

check-format:
    clang-format --dry-run --Werror `rg --files src -g '*.{c,cc,cpp,cxx,h,hh,hpp,hxx}'`

run-release: build-release
    cmake --build --preset build-release --target run_app

run-sanitize: build-sanitize
    ASAN_OPTIONS=halt_on_error=1 UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 cmake --build --preset build-sanitize --target run_app
════ presets ════
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
      "configurePreset": "cfg-release"
    },
    {
      "name": "build-profile",
      "displayName": "App Build Release With Debug Info",
      "configurePreset": "cfg-profile"
    },
    {
      "name": "build-sanitize",
      "displayName": "App Build With ASan + UBSan",
      "configurePreset": "cfg-sanitize"
    }
  ]
}
════ Sanitizers.cmake ════
include_guard(GLOBAL)

option(FIRE_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(FIRE_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

function(fire_enable_sanitizers targetName)
    if(NOT TARGET "${targetName}")
        message(FATAL_ERROR "Cannot enable sanitizers for unknown target: ${targetName}")
    endif()

    if(NOT FIRE_ENABLE_ASAN AND NOT FIRE_ENABLE_UBSAN)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        if(FIRE_ENABLE_UBSAN)
            message(FATAL_ERROR "UndefinedBehaviorSanitizer is not supported by the MSVC compiler frontend")
        endif()

        target_compile_options("${targetName}" PRIVATE /fsanitize=address)
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
        message(FATAL_ERROR
            "Sanitizers are not supported for C++ compiler ${CMAKE_CXX_COMPILER_ID}"
        )
    endif()

    set(enabledSanitizers)
    if(FIRE_ENABLE_ASAN)
        list(APPEND enabledSanitizers address)
    endif()
    if(FIRE_ENABLE_UBSAN)
        list(APPEND enabledSanitizers undefined)
    endif()
    list(JOIN enabledSanitizers "," sanitizerList)

    target_compile_options("${targetName}" PRIVATE
        "-fsanitize=${sanitizerList}"
        -fno-omit-frame-pointer
    )
    target_link_options("${targetName}" PRIVATE "-fsanitize=${sanitizerList}")

    if(FIRE_ENABLE_UBSAN)
        target_compile_options("${targetName}" PRIVATE -fno-sanitize-recover=undefined)
    endif()
endfunction()
════ removed fps meter (34d7ffa) ════
34d7ffa Refactoring: remove naive fps meter
 src/app/FireWidget.cpp | 36 ------------------------------------
 src/app/FireWidget.hpp |  8 --------
 src/app/MainWindow.cpp |  5 +----
 3 files changed, 1 insertion(+), 48 deletions(-)
```

</details>

**🔧 Tool call — `Bash`**

*Check composition branch MainWindow*

```bash
echo "════ refactor-composition MainWindow ════"; git show 122e046:src/app/MainWindow.hpp; echo "---"; git show 122e046:src/app/MainWindow.cpp; echo "════ FireController.hpp there ════"; git show 122e046:src/app/FireController.hpp
```

<details>
<summary>Result</summary>

```
════ refactor-composition MainWindow ════
#pragma once

#include <QMainWindow>

class PerformanceMonitor;

class MainWindow final : public QMainWindow {
public:
    // The monitor is owned above the window so it outlives the widget tree: Qt
    // deletes child objects from ~QObject, which runs after this class's own
    // members are gone.
    explicit MainWindow(PerformanceMonitor& performanceMonitor, QWidget* parent = nullptr);
};
---
#include "app/MainWindow.hpp"

#include "app/ControlPanel.hpp"
#include "app/FireController.hpp"
#include "app/FirePalette.hpp"
#include "app/FireWidget.hpp"
#include "app/PerformanceMonitor.hpp"
#include "app/PerformancePanel.hpp"

#include <QKeySequence>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

#include <cstddef>

namespace {
// Used at exactly one site: the widget derives its geometry from the frames it
// receives, so there is nowhere for a second copy to drift from.
constexpr std::size_t SIMULATION_WIDTH = 800;
constexpr std::size_t SIMULATION_HEIGHT = 600;
} // namespace

MainWindow::MainWindow(PerformanceMonitor& performanceMonitor, QWidget* const parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Fire Demo"));

    auto* const centralWidget = new QWidget(this);
    auto* const windowLayout = new QVBoxLayout(centralWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    auto* const fireController = new FireController(SIMULATION_WIDTH, SIMULATION_HEIGHT, performanceMonitor, this);
    auto* const fireWidget = new FireWidget(FirePalette::classic(), performanceMonitor, centralWidget);

    // Present once before layout so the widget's size hints describe a real frame
    // the first time Qt asks for them.
    fireWidget->present(fireController->heat());
    windowLayout->addWidget(fireWidget, 1);

    auto* const controlPanel = new ControlPanel(fireController->parameters(), centralWidget);
    windowLayout->addWidget(controlPanel);
    auto* const performancePanel = new PerformancePanel(performanceMonitor, centralWidget);
    windowLayout->addWidget(performancePanel);
    setCentralWidget(centralWidget);

    connect(controlPanel, &ControlPanel::toggleRequested, fireController, &FireController::toggleRunning);
    connect(controlPanel, &ControlPanel::resetRequested, fireController, &FireController::reset);
    connect(controlPanel, &ControlPanel::sourceHeatChanged, fireController, &FireController::setSourceHeat);
    connect(controlPanel, &ControlPanel::coolingChanged, fireController, &FireController::setCooling);
    connect(fireController, &FireController::frameReady, fireWidget, [fireController, fireWidget] {
        fireWidget->present(fireController->heat());
    });
    connect(fireController, &FireController::runningChanged, controlPanel, [controlPanel](const bool running) {
        controlPanel->setPaused(!running);
    });
    connect(fireController, &FireController::runningChanged, performancePanel, [performancePanel](const bool running) {
        performancePanel->setPaused(!running);
    });

    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    connect(pauseShortcut, &QShortcut::activated, fireController, &FireController::toggleRunning);
    connect(resetShortcut, &QShortcut::activated, fireController, &FireController::reset);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    fireController->run();
    resize(960, 720);
}
════ FireController.hpp there ════
#pragma once

#include "sim/FireSimulation.hpp"
#include "sim/HeatFrame.hpp"

#include <QObject>
#include <QTimer>

#include <chrono>
#include <cstddef>

class PerformanceMonitor;

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(std::size_t simulationWidth,
                            std::size_t simulationHeight,
                            PerformanceMonitor& performanceMonitor,
                            QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    [[nodiscard]] HeatFrame heat() const noexcept { return simulation.heat(); }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }

public slots:
    void run();
    void pause();
    void toggleRunning();
    void reset();
    void setSourceHeat(int sourceHeat);
    void setCooling(int cooling);

signals:
    void frameReady();
    void runningChanged(bool running);

private slots:
    void advanceFrame();

private:
    using Clock = std::chrono::steady_clock;

    static constexpr int WAKE_INTERVAL_MILLISECONDS = 16;
    static constexpr int SIMULATION_TICKS_PER_SECOND = 60;
    static constexpr int MAX_TICKS_PER_WAKE = 3;
    static constexpr std::chrono::duration<double> SIMULATION_STEP{1.0 / SIMULATION_TICKS_PER_SECOND};

    FireSimulation simulation;
    PerformanceMonitor& performanceMonitor;
    QTimer wakeTimer;
    Clock::time_point elapsedTimeReference;
    std::chrono::duration<double> accumulatedTime{0.0};
};
```

</details>

🤝 Following project rules.

I read all 765 lines of source, the build files, and — since your question is literally "what happens when we add performance monitoring" — the two branches where that experiment was already run (`perf-monitor-cdx`, `refactor-composition`). Everything below is about `main` @ `24fa3eb`; the branches I use only as evidence.

---

# Architecture review — decomposition & extendability

## Current shape

```
fire_simulation (pure C++, no Qt link)     classic_fire (Qt executable)
  FireParameters   self-clamping values      MainWindow    composition root + all wiring
  FireSimulation   heatMap + tick/reset      FireController timing + sim ownership + param facade
                                             FireWidget    palette + heat→RGB + paint + letterbox
                                             ControlPanel  widgets + intent signals
```

Data flow is one-way and clean: `ControlPanel —intent→ FireController —frameReady→ FireWidget`. No view reaches back into the simulation.

## What holds up

- **The Qt boundary is real, not aspirational.** `fire_simulation` doesn't link Qt ([CMakeLists.txt:14](CMakeLists.txt:14)), so AGENTS.md rule 3 is enforced by the build, not by discipline. This is the single best structural decision in the repo.
- **`FireParameters` as a self-clamping value object that also publishes its bounds** ([FireParameters.hpp:10](src/sim/FireParameters.hpp:10)) lets `ControlPanel` derive slider ranges from the model instead of duplicating them ([ControlPanel.cpp:56](src/app/ControlPanel.cpp:56)). The comment justifying the mutable `parameters()` accessor ([FireSimulation.hpp:21](src/sim/FireSimulation.hpp:21)) is an honest invariant argument, and it's correct.
- **Ownership is unambiguous** — Qt parenting throughout, `std::vector`/`std::span`/`std::array`, no owning raw pointers, no manual cleanup.
- **The simulation is deterministic** (fixed seed, no hidden state, `tick()` re-reads parameters each frame). That's a gift for testing — currently unused.

---

## Findings, ranked by extendability cost

### 1. The pipeline emits no events — only a doorbell

`frameReady()` ([FireController.hpp:32](src/app/FireController.hpp:32)) carries no payload. Everything a consumer might want to know about a frame — how many ticks ran, how long they took, whether we fell behind — stays local to `advanceFrame()`.

The sharpest case is [FireController.cpp:57-60](src/app/FireController.cpp:57): when the accumulator exceeds `MAX_TICKS_PER_WAKE`, time is **silently discarded**. That clamp is precisely the "we are missing our budget" event any monitor exists to report, and it is unobservable from outside the function.

Consequence: a component that wants facts about frames cannot *subscribe*; it must be *injected into the producer*. That is the root cause of finding 2.

### 2. No home for cross-cutting concerns, and no seam to attach them

`src/app/` is a flat bag of four Qt classes. There is no `metrics/`, `diagnostics/`, or `platform/` module — anything that is neither "the simulation" nor "a widget" has nowhere to live. Worse, a monitor placed in `src/app/` **can never measure inside `fire_simulation`**, because the dependency runs app→sim and reversing it would break the Qt boundary.

So per-stage timing inside the simulation isn't merely awkward today — it is structurally unavailable. It requires a *third, Qt-free* module that both sides may depend on. There's a stale `libfire_metrics.a` sitting in `build/` from exactly that realisation on another branch.

### 3. `FireWidget` is three classes wearing one hat

[FireWidget.hpp:23-27](src/app/FireWidget.hpp:23) holds palette generation (`makePalette`), heat→RGB conversion (`present`), and Qt painting + aspect-fit math (`paintEvent`/`fittedFrameRect`). None is independently replaceable, testable, or measurable. You cannot swap the palette, unit-test the conversion loop (it's pure integer work over a span!), or time the conversion without editing the widget.

### 4. Geometry travels separately from the data it describes

`SIMULATION_WIDTH/HEIGHT` are handed independently to two different objects ([MainWindow.cpp:21-22](src/app/MainWindow.cpp:21)) — the `// TODO: remove duplicate constants` on line 20 already concedes this. The agreement is then re-checked at runtime with `assert` plus a silent `return` ([FireWidget.cpp:38-42](src/app/FireWidget.cpp:38)): in a release build, a geometry mismatch is a dropped frame with no diagnostic whatsoever. That's the ad-hoc error semantics AGENTS.md warns against.

Related: the `QImage` is sized once at construction ([FireWidget.cpp:29](src/app/FireWidget.cpp:29)) with no reallocation path, so a runtime-resizable simulation is currently impossible.

### 5. `FireController` carries three roles, one of which scales badly

Timing/scheduling, simulation ownership, and a per-parameter forwarding facade ([FireController.cpp:44-50](src/app/FireController.cpp:44) — `int`→`uint8_t` adapters bridging Qt's slot types to the sim's).

Adding one scalar parameter costs five edits across four files: field+clamp in `FireParameters`, slot declaration + definition in `FireController`, signal + slider in `ControlPanel`, `connect` in `MainWindow`. Linear shotgun surgery on the axis the app is most likely to grow along.

### 6. Parameter binding is one-way

`ControlPanel` reads initial values once in its constructor ([ControlPanel.cpp:58,65](src/app/ControlPanel.cpp:58)) and never re-reads, while `FireSimulation::parameters()` is mutable to anyone holding the simulation. Today only the panel writes, so it works. The moment a second writer appears — presets, a config file, a keyboard shortcut, a demo mode — the sliders silently desynchronise. Only `setPaused` currently has a push-back path.

### 7. Layering is directory convention, not a build constraint

`target_include_directories(fire_simulation PUBLIC src)` ([CMakeLists.txt:21](CMakeLists.txt:21)) publishes the *entire* `src` tree as the library's public include path. Nothing stops a `sim` header from including an `app` header — only the Qt *link* boundary is enforced. With two modules this is survivable; with a third it stops being.

### 8. No tests, no test target

No CTest, no test preset, no test directory. `fire_simulation` is pure, deterministic, and trivially testable — and untested. This is the finding that gates all the others: every restructuring below is a refactor performed without a safety net.

---

## The concrete test: adding performance monitoring

This was actually attempted on `perf-monitor-cdx`. The result: **415 lines added, 7 existing files modified, 2 breaking constructor changes** — for a feature that observes and changes nothing.

What the pressure test exposed:

| Symptom on that branch | Which finding caused it |
|---|---|
| `FireController` ctor gains `PerformanceMonitor&`; every call site breaks | 1 — no subscription, must inject |
| `FireWidget` ctor gains `PerformanceMonitor&`; every call site breaks | 1, 3 |
| `MainWindow.hpp` must `#include` the monitor and own it as a member | 2 — no lifetime home, so the composition root absorbs it |
| Four hand-written `Clock::now()` / subtract / `record` sites inside existing methods | 3 — no measurable seams |
| `paintEvent` needed an **artificial inner `{ }` scope** so `~QPainter` landed inside the timed region | no RAII timer existed; the follow-up branch had to add `ScopedMetric` |
| `clearMeasurements()` / `reset()` threaded into `run()` and `reset()` | monitor lifecycle entangled with the controller's state machine |
| Catch-up/dropped-tick data required new counters *inside* `advanceFrame` | 1 — the clamp emits nothing |
| Monitor sits in `src/app`, so the simulation's internals stay unmeasurable | 2 |

The follow-up branch `refactor-composition` fixed the right things — `HeatFrame` (finding 4), `FirePalette`/`FireRenderer` split (finding 3), a Qt-free `fire_metrics` library (finding 2), `ScopedMetric` for the scope problem. Notably it did **not** fix findings 1, 5, or 8: `PerformanceMonitor&` is still threaded through both constructors, `MainWindow` is still the hand-wired root, and there are still no tests. That tells you the injection coupling is the deeper issue, and it survives a decomposition-only refactor.

---

## Recommendations, in dependency order

1. **Add a test target and cover `fire_simulation` first.** Everything else is a refactor; do this before, not after. Deterministic seed makes golden-frame tests cheap.
2. **Give the frame signal a payload.** `frameReady(FrameInfo)` carrying tick count, step duration, and a *discarded-time* flag turns findings 1 and half of 2 into subscriptions instead of constructor surgery. Emit an explicit event when the catch-up clamp fires — that's a real behaviour, currently invisible.
3. **Introduce `HeatFrame`** (data + geometry as one value). Deletes the duplicated constants, the runtime size check, the silent-return failure mode, and unblocks a resizable simulation. The version on `refactor-composition` is well-judged — the constructor-established invariant is the right idea.
4. **Split `FireWidget`** into palette / renderer / widget. Each becomes testable and measurable without instrumentation inside it.
5. **Add a Qt-free `fire_metrics` module** that both `sim` and `app` may depend on — not an `app`-local monitor. This is the only shape that permits measuring inside the simulation.
6. **Replace the per-parameter facade** with a single `applyParameters(FireParameters)` / change-notification pair, closing findings 5 and 6 together and making the panel a two-way binding.
7. **Tighten include scoping** — narrow the `PUBLIC src` include path so layering is enforced, not merely documented.
8. **Extract composition out of `MainWindow`.** A plain non-Qt wiring type gives you one place to add participants and, incidentally, something you can construct in a test.

Items 1–3 give the largest return; 5 is the one that specifically unblocks the monitoring question.

---

I made no changes and ran no builds. Want me to publish this as a shareable artifact, or turn any of the eight recommendations into a concrete implementation plan?

## 👤 User  
*2026-08-25 15:50:11.820 UTC*

Let's sketch what new classes might emerge, what are they responsible for, how they are related. I'm specifically interested in designing rendering pipeline, that follows good patterns of real-time graphics applications, even if simplified for the scope we have.

## 🤖 Assistant  
*2026-08-25 15:53:06.132 UTC*

Here's the design. I'll lead with the pipeline since that's your focus, then the surrounding types, then what I'd deliberately leave unbuilt.

# Rendering pipeline design

## Layering

```
┌─ app/ ───────────────── Qt only lives here ─────────────────────┐
│  MainWindow · FireController · ControlPanel · FireView          │
│  StatsPanel                                                      │
└────────────────────────────┬────────────────────────────────────┘
                             │ drives
┌─ engine/ ──────────────────▼── pure C++: the frame loop ────────┐
│  FireEngine · FrameClock · FrameReport                           │
└──────────┬────────────────────────────────┬─────────────────────┘
           │                                │
┌─ sim/ ───▼──── pure C++ ───────┐  ┌─ render/ ▼─── pure C++ ─────┐
│ FireSimulation · FireParameters │  │ RenderPipeline · RenderPass │
│ HeatFrame                       │  │ HeatShadePass · FirePalette │
└─────────────────────────────────┘  │ PixelBuffer · RenderContext │
                                     │ RenderSettings · Viewport   │
                                     └─────────────────────────────┘
        ┌─ metrics/ ─── pure C++, dependency of engine & render ───┐
        │  TimeSeriesMetric · CounterMetric · ScopedTimer          │
        │  FrameProfiler · MetricHandle                            │
        └─────────────────────────────────────────────────────────┘
```

Dependency rule: `app → engine → {sim, render} → metrics`. Four static libs plus the executable; only the executable links Qt. That's the same enforcement trick you already use for `fire_simulation`, extended to the whole spine.

## The frame, end to end

```
QTimer wake (≈16 ms)
  │
  ▼ FireController::onWake()
    elapsed = Clock::now() - lastWake
  │
  ▼ FireEngine::advance(elapsed) ─────────────────────────► FrameReport
    │
    ├─ FrameClock::consume(elapsed) → TickPlan{ticks, alpha, discardedTime}
    │
    ├─ [Simulate]  for each tick: FireSimulation::tick()      ⟵ timed
    │
    ├─ [Extract]   HeatFrame frame = simulation.heat()
    │
    └─ RenderPipeline::execute(RenderContext{frame, back, settings, alpha})
         ├─ [Shade]  HeatShadePass — LUT map heat → PixelBuffer  ⟵ timed
         ├─ [Post]   … future passes …                          ⟵ timed
         └─ swap(back, front)
  │
  ▼ emit frameReady(FrameReport)
    │
    ├──► FireView::present(engine.frame())   [Composite / Present]
    │      wrap front buffer as QImage (no copy) → update()
    │      paintEvent: fillRect + drawImage(Viewport::fit(...))   ⟵ timed
    │
    └──► StatsPanel::update(engine.profiler().snapshot())
```

The four stage names — **Simulate, Extract, Shade, Present** — are the whole point. They're the units of measurement, the units of replacement, and (later) the units of parallelism.

---

## Render classes

### `PixelBuffer` — the render target

```cpp
using Rgba32 = std::uint32_t;   // 0xFFRRGGBB

class PixelBuffer final {
public:
    void resize(std::size_t width, std::size_t height);
    [[nodiscard]] std::size_t width() const noexcept;
    [[nodiscard]] std::size_t height() const noexcept;
    [[nodiscard]] std::span<Rgba32> row(std::size_t y) noexcept;
    [[nodiscard]] const Rgba32* data() const noexcept;
private:
    std::vector<Rgba32> pixels;
    std::size_t bufferWidth{0};
    std::size_t bufferHeight{0};
};
```

Owns target memory and its geometry together — the same invariant `HeatFrame` gives the input side. The packing is byte-identical to `QImage::Format_RGB32`, so the Qt layer wraps it with the non-owning `QImage(const uchar*, w, h, format)` constructor and never copies.

Analog: swapchain image / framebuffer.

### `FirePalette` — the LUT

```cpp
class FirePalette final {
public:
    static FirePalette classic();                          // today's PALETTE_STOPS
    static FirePalette fromStops(std::span<const PaletteStop>);
    [[nodiscard]] Rgba32 operator[](std::uint8_t heat) const noexcept;
private:
    std::array<Rgba32, 256> entries;
};
```

A 1-D transfer function, value semantics, no Qt. In a GPU port this is literally a 256×1 LUT texture. Testable in isolation: endpoints correct, no unwritten entries, luminance monotone — none of which you can assert today because it's a private static inside a widget.

### `RenderContext` — the per-frame blackboard

```cpp
struct RenderContext {
    const HeatFrame& heat;
    PixelBuffer& target;
    const RenderSettings& settings;
    double interpolationAlpha{0.0};
    std::uint64_t frameIndex{0};
};
```

Why a struct rather than parameters: adding a new pipeline input later is **one field**, not a signature change rippling through every pass. This is the frame-graph resource registry, shrunk to what 800×600 needs.

### `RenderPass` — one named stage

```cpp
class RenderPass {
public:
    virtual ~RenderPass() = default;
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    virtual void execute(RenderContext&) = 0;
};
```

The `name()` is load-bearing, not cosmetic: because a pass can identify itself, the pipeline registers a profiler channel for it automatically. **Instrumentation stops being a per-call-site edit** — which is the exact failure mode `perf-monitor-cdx` hit.

### `HeatShadePass` — the only pass you need on day one

Resizes the target to match the incoming `HeatFrame`, then LUT-maps heat → pixels. It is `FireWidget::present`'s inner loop, lifted out and made Qt-free. Roughly 20 lines.

### `RenderPipeline` — pass order + swapchain

```cpp
class RenderPipeline final {
public:
    explicit RenderPipeline(FrameProfiler& profiler);

    void addPass(std::unique_ptr<RenderPass> pass);   // registers its metric channel
    void execute(const HeatFrame&, const RenderSettings&, double alpha);

    [[nodiscard]] const PixelBuffer& front() const noexcept;

private:
    struct Stage {
        std::unique_ptr<RenderPass> pass;
        MetricHandle metric;
    };
    std::vector<Stage> stages;
    PixelBuffer backBuffer;
    PixelBuffer frontBuffer;
    FrameProfiler& profiler;
    std::uint64_t frameIndex{0};
};
```

`execute` is: for each stage, `ScopedTimer timer{profiler.channel(stage.metric)}; stage.pass->execute(context);` then swap. Adding a glow or scanline effect is one `addPass` line at composition, and it arrives pre-instrumented.

On double buffering — single-threaded, it isn't strictly required. I'd still take it, for two reasons: `paintEvent` can fire independently of the timer (resize, expose) and must never read a half-written buffer; and it's the seam that lets rendering move off the UI thread later without touching a single pass.

---

## Engine classes

### `FrameClock` — fixed timestep, isolated

```cpp
struct TickPlan {
    int ticks{0};
    double interpolationAlpha{0.0};
    Duration discardedTime{};    // non-zero ⇒ we could not keep up
};

class FrameClock final {
public:
    FrameClock(int ticksPerSecond, int maximumTicksPerWake);
    [[nodiscard]] TickPlan consume(Duration elapsed) noexcept;
    void reset() noexcept;
};
```

`discardedTime` is the important field. It turns [FireController.cpp:57-60](src/app/FireController.cpp:57)'s silent accumulator clamp into a **returned fact**. That clamp is the single most valuable signal a performance monitor could have, and right now it's invisible outside a local function. And this class is where fixed-step logic finally becomes testable — feed it synthetic elapsed times, assert the tick counts.

### `FireEngine` — the frame loop and the resource owner

```cpp
class FireEngine final {
public:
    FireEngine(std::size_t width, std::size_t height);

    FrameReport advance(Duration elapsed);
    void reset();

    [[nodiscard]] const PixelBuffer& frame() const noexcept;
    [[nodiscard]] FireParameters& parameters() noexcept;
    void setRenderSettings(const RenderSettings&) noexcept;
    [[nodiscard]] const FrameProfiler& profiler() const noexcept;

private:
    FireSimulation simulation;
    FrameClock clock;
    FrameProfiler frameProfiler;
    RenderPipeline pipeline;
    RenderSettings renderSettings;
};
```

This is the class that dissolves your original question. The profiler lives *here*, alongside everything it measures, so nothing gets threaded through Qt constructors and `MainWindow.hpp` never learns that monitoring exists. It's also fully testable headlessly: call `advance()` sixty times, assert on `frame()` and the reports.

`FrameReport { int ticksExecuted; Duration discardedTime; std::uint64_t frameIndex; }` is the payload `frameReady()` currently lacks.

---

## Metrics classes

```cpp
class MetricHandle { std::size_t index; };   // resolved once, at pipeline build

class TimeSeriesMetric { void record(Duration); MetricStatistics statistics() const; };
class CounterMetric    { void add(std::uint64_t = 1); std::uint64_t value() const; };
class ScopedTimer      { explicit ScopedTimer(TimeSeriesMetric&); ~ScopedTimer(); };

class FrameProfiler final {
public:
    MetricHandle registerChannel(std::string_view name);  // once
    TimeSeriesMetric& channel(MetricHandle) noexcept;     // per frame, O(1) index
    [[nodiscard]] ProfileSnapshot snapshot() const;       // iterable: name + stats
};
```

Two differences from the `PerformanceMonitor` on `perf-monitor-cdx`, both deliberate:

**Channels are registered, not hard-coded.** That version had `FrameMetric simulationTick; heatToImage; paintEvent;` as three named members plus a matching `Snapshot` struct plus matching panel rows. A fourth stage meant editing all three. With registration the profiler never learns stage names at compile time, and `StatsPanel` iterates the snapshot — so a new pass produces a new row with no UI change.

**Lookup is a pre-resolved index, not a string.** Registration happens once at pipeline construction; the per-frame path is an array index. That's the standard real-time profiler shape (GPU timestamp query slots work exactly this way) and it keeps the measurement out of the measurement.

`ScopedTimer` also removes the thing that branch had to do in `paintEvent` — introducing an artificial `{ }` block so `~QPainter` fell inside the timed region. With RAII that's the natural spelling rather than a workaround.

---

## Qt-side classes

| Class | Responsibility after the change |
|---|---|
| `FireController` | Qt adapter, nothing else. Owns `QTimer` + `FireEngine`; run/pause/toggle/reset; `emit frameReady(FrameReport)`. ~40 lines. |
| `FireView` (was `FireWidget`) | Presentation surface only. `present(const PixelBuffer&)` wraps + `update()`; `paintEvent` blits into `Viewport::fit(...)`. No palette, no conversion, no simulation constants. |
| `Viewport` | The letterbox math from `fittedFrameRect`, as a pure function. Testable; belongs in `render/`. |
| `ControlPanel` | Binds sliders to `FireParameters` and `RenderSettings`; emits `parametersChanged(FireParameters)` / `renderSettingsChanged(RenderSettings)`. |
| `StatsPanel` | Renders `ProfileSnapshot` generically, one row per channel. |
| `MainWindow` | Wiring only. No simulation dimensions — the view derives geometry from the buffers it receives. |

Note the `ControlPanel` change: **two** controller slots total instead of one per knob. Adding a parameter becomes a struct field plus one slider registration — two places, not the five files it costs today. It also makes the binding two-way, so sliders can't go stale when something else writes the parameters.

---

## How this lands against the review findings

| # | Finding | Resolved by |
|---|---|---|
| 1 | Pipeline emits no events | `FrameReport` payload + `TickPlan::discardedTime` |
| 2 | No home / seam for cross-cutting concerns | `metrics/` as a Qt-free lib both sides may depend on; `FireEngine` as the owner |
| 3 | `FireWidget` is three classes | `FirePalette` + `HeatShadePass` + `FireView` |
| 4 | Geometry travels apart from data | `HeatFrame` in, `PixelBuffer` out — both carry their own dimensions |
| 5 | `FireController` is three roles | `FrameClock` + `FireEngine` + flattened settings |
| 6 | One-way parameter binding | `FireParameters`/`RenderSettings` as round-tripped values |
| 7 | Layering not enforced | Four targets with narrowed include scopes |
| 8 | No tests | Everything except `app/` is now headless-testable |

---

## What I would deliberately *not* build

Naming these matters as much as the design, since AGENTS.md rules out speculative frameworks:

- **Threaded rendering.** The double buffer leaves the seam. Don't cross it.
- **Frame interpolation.** `interpolationAlpha` is plumbed through `RenderContext` and ignored by `HeatShadePass`. One `double`, zero cost, and the day you want motion smoothing the change is local to one pass.
- **A real frame graph** with declared resources, aliasing, and dependency sorting. A `vector<unique_ptr<RenderPass>>` in fixed order is the right amount of frame graph for a CPU raster of this size.
- **Post-process passes** (glow, bloom, scanlines). The pass list makes them cheap later; building them now is inventing requirements.
- **A GPU path.** Worth knowing that `PixelBuffer` + `FirePalette` map 1:1 onto a texture upload and a LUT sampler if you ever want `QOpenGLWidget`, but nothing in the design should be shaped around that today.

## One real judgment call

**Should `render/` be Qt-free, or just use `QImage` directly** (as `FireRenderer` does on your `refactor-composition` branch)?

I've sketched it Qt-free because "the renderer doesn't know about the windowing system" is the load-bearing separation in real-time graphics, and it makes the entire pixel pipeline unit-testable. The cost is one wrapping step at the Qt boundary and a lifetime constraint: `FireView`'s `QImage` is a non-owning view over the engine's front buffer, so the engine must outlive the widget tree — the same hazard your branch already documented for `PerformanceMonitor`.

If that constraint bothers you, `QImage::copy()` at present time removes it entirely for the price of one memcpy per frame, and `render/` linking Qt is a defensible simplification. I'd take Qt-free, but it's genuinely a call and not a correctness matter.

## Suggested build order

Each step ships independently and leaves the app working:

1. `metrics/` + its tests — no callers yet, pure addition
2. `HeatFrame`, `PixelBuffer`, `FirePalette`, `HeatShadePass`, `RenderPipeline` + tests — `FireWidget` loses its conversion code
3. `FrameClock` + tests — `FireController` loses the accumulator
4. `FireEngine` — `FireController` loses everything else
5. Settings flattening in `ControlPanel`
6. `StatsPanel`

Step 1 first specifically because it's the only one that's pure addition, and it gives you the test target that makes steps 2–4 safe.

---

Want me to turn this into a shareable design doc as an artifact (with a proper rendered pipeline diagram), or drill into one of these — the pass/profiler interaction and the settings flattening are the two with the most detail still unspecified?

## 👤 User  
*2026-08-25 16:13:32.617 UTC*

I like this structure. I think I'd like to discus - given the context of the demo app, full size pipeline abstraction (RenderContext, RenderPipeline, RenderPass, back- and front buffer, stages) feel like an overkill. If we commit that scope is limited only to cpu rendering, can we simplify rendering abstraction and do not compromise the rest of the design?

## 🤖 Assistant  
*2026-08-25 16:14:52.770 UTC*

Your instinct is right, and I'd push it further than you're proposing. Let me be precise about what each piece was actually buying, because one of them was buying nothing at all.

## What each abstraction bought, honestly

**`RenderPass` (virtual interface)** — bought generic instrumentation via `name()`, runtime-composable ordering, and a polymorphic extension point. Ordering and polymorphism are speculative with exactly one pass: a `vector<unique_ptr<>>` of one element with a vtable is ceremony. The instrumentation win was real — but see below, it was solving a problem you don't have.

**`RenderContext`** — bought signature stability across N passes. With one render function, the parameters *are* the signature and changing it is one call site. Cut.

**`RenderPipeline`** — bought pass ordering + target ownership + per-pass timing. With one pass it collapses into the renderer itself. Cut.

**Front/back buffers** — this one I over-included, and the argument against is stronger than "it's speculative". `QWidget::update()` posts a *deferred* paint request; `paintEvent` therefore runs in a later event-loop iteration, never nested inside the timer slot. `FireEngine::advance()` runs to completion within that slot and spins no nested event loop. So the write and the read are strictly serialized by the event loop — **tearing is not possible**, and the second buffer protects against nothing. (That holds as long as nothing calls synchronous `repaint()` from the render path or opens a modal mid-render. Neither does.) Cut.

**`PixelBuffer`, `FirePalette`** — not pipeline machinery. These are the input/output contract and a real value type with testable content. Keep both.

## The insight that makes the cut safe

The pass interface's instrumentation argument was: *"if stages self-identify, nobody has to hand-write timing at each site."* That's the fix for `perf-monitor-cdx`'s actual failure — timing code scattered inside `FireController` and `FireWidget` because there was no single place that ran the stages.

But `FireEngine::advance()` **is** that place. Once it exists, you get the same property without polymorphism:

```cpp
FrameReport FireEngine::advance(const Duration elapsed) {
    const TickPlan plan = clock.consume(elapsed);
    {
        ScopedTimer timer{frameProfiler.channel(simulateChannel)};
        for (int tick = 0; tick < plan.ticks; ++tick) {
            simulation.tick();
        }
    }
    {
        ScopedTimer timer{frameProfiler.channel(shadeChannel)};
        renderer.render(simulation.heat());
    }
    return {plan.ticks, plan.discardedTime, ++frameIndex};
}
```

Two timing lines, in one file, at the orchestration point — which is exactly where instrumentation belongs when an orchestrator exists. The pass interface was buying a property the engine already gives you for free.

## Simplified `render/` — four types, ~90 lines, zero virtuals

```cpp
// render/PixelBuffer.hpp — owns target memory + geometry (unchanged)
using Rgba32 = std::uint32_t;                    // 0xFFRRGGBB == QImage::Format_RGB32
class PixelBuffer final { resize · width · height · row · data };

// render/FirePalette.hpp — the 256-entry LUT (unchanged)
class FirePalette final { static classic() · fromStops() · operator[] };

// render/Viewport.hpp — the letterbox math, lifted out of the widget
struct FitRect { int x, y, width, height; };
[[nodiscard]] constexpr FitRect fitPreservingAspect(
    int availableWidth, int availableHeight, int sourceWidth, int sourceHeight) noexcept;

// render/FireRenderer.hpp — replaces Pipeline + Pass + Context
class FireRenderer final {
public:
    explicit FireRenderer(FirePalette palette) noexcept;

    void setPalette(const FirePalette& palette) noexcept;
    void render(const HeatFrame& heat);                  // resizes target, LUT-maps

    [[nodiscard]] const PixelBuffer& target() const noexcept;

private:
    FirePalette palette;
    PixelBuffer buffer;
};
```

`render()` is ~12 lines: resize the buffer if the frame geometry changed, then the double loop from today's `FireWidget::present` with `palette[heat]` instead of an inline array. No allocation after the first frame. It's a pure function with a cache, and it's headlessly testable.

## Revised frame flow

```
QTimer wake
  ▼ FireController::onWake()
  ▼ FireEngine::advance(elapsed) ──────────────► FrameReport
      ├─ FrameClock::consume  → TickPlan{ticks, alpha, discardedTime}
      ├─ [Simulate]  N × FireSimulation::tick()          ⟵ ScopedTimer
      └─ [Shade]     FireRenderer::render(heat())        ⟵ ScopedTimer
  ▼ emit frameReady(FrameReport)
      ├──► FireView::present(engine.frame())
      │      wrap PixelBuffer as QImage (no copy) → update()
      │      paintEvent: fillRect + drawImage(fitPreservingAspect(...))  ⟵ ScopedTimer
      └──► StatsPanel::update(engine.profiler().snapshot())
```

Three named stages instead of a variable list. `FireEngine` registers `Simulate` and `Shade` in its constructor; `FireView` takes a single `TimeSeriesMetric&` for `Present`.

That last bit is worth flagging: it's the one place a metrics type still crosses into a widget constructor. But it's one narrow reference to a leaf value type meaning *"record durations here"* — not a monitor that knows every stage name. That's the distinction that made `perf-monitor-cdx` painful, and this side of it is fine.

## Does it compromise the rest? No — and here's why structurally

Nothing outside `render/` can tell the difference. `FireEngine` calls `renderer.render(heat)`; `FireView` receives a `const PixelBuffer&`. Both are true in either design.

| Layer | Changes from the cut |
|---|---|
| `sim/` — `HeatFrame`, simulation | none |
| `metrics/` — all types | none (stage registration moves from passes to engine) |
| `engine/` — `FrameClock`, `FireEngine`, `FrameReport` | none |
| `app/` — controller, view, panels | none |

The pipeline machinery was a **leaf**. The things that are expensive to change later are the *boundaries* — `HeatFrame` in, `PixelBuffer` out, engine owns the loop, only `app/` links Qt — and every one of those survives untouched. Interior polymorphism is cheap to add and cheap to remove; boundaries are not. You're cutting the cheap half.

All eight review findings still resolve the same way. Finding 3 (`FireWidget` is three classes) is still fixed — palette, shading, and presentation are still three types, just without a vtable between them.

## What you actually give up

1. **Adding a post-process effect** means editing `FireRenderer::render` rather than adding one `addPass` line. Local, and you'd be reading that function anyway.
2. **Per-effect timing granularity** if `render()` ever grows several steps. Fix at that time: a second `ScopedTimer` inside `render()`, or split into two channels. Two lines.
3. **Moving rendering off the UI thread** would need double buffering back. Fix at that time: internal to `FireRenderer`, since `PixelBuffer` is already the unit you'd double.

Every one is local to `render/` and reversible without touching another module. If a second effect ever shows up and the ordering genuinely starts to matter, reintroducing `RenderPass` is a `render/`-internal refactor — the engine and the view never learn it happened.

## Revised inventory

| | |
|---|---|
| **Cut** | `RenderPass`, `RenderContext`, `RenderPipeline`, front/back buffers |
| **Keep** | `PixelBuffer`, `FirePalette`, `FireRenderer`, `Viewport` — 4 types, ~90 lines, no virtuals |
| **Unchanged** | `HeatFrame`, all of `metrics/`, `FrameClock`, `FireEngine`, `FrameReport`, all `app/` classes |

## Two more I'd challenge while we're here

**Defer `RenderSettings`.** It had two fields, and one of them (`smoothScaling`) is consumed by the view, not the renderer. Today there's exactly one plausible render knob — palette choice — and `FireRenderer::setPalette` covers it. Introduce the struct when a second knob appears. (This doesn't weaken finding 6: that was about *simulation* parameters going stale, which `FireParameters` round-tripping fixes independently.)

**`FrameProfiler`'s registration is the next thing worth questioning.** With three fixed channels you could hard-code three members and skip `MetricHandle` entirely. The one thing registration still buys is that `StatsPanel` iterates `snapshot()` generically, so channels can come and go without touching Qt code. I'd keep it for that reason alone — but it's a fair trade to look at, and it's the only remaining piece where I'd say "this earns its keep by a margin rather than clearly."

Want me to write out `FireRenderer::render` and `FireEngine::advance` in full so the shape is concrete, or move on to how `ControlPanel` and `StatsPanel` bind?

## 👤 User  
*2026-08-25 16:30:19.364 UTC*

Let's create docs/render_pipeline_redesign.md and save there proposed new architecture. Use simplified renderer design, drop RenderSettings, keep FrameProfiler.

## 🤖 Assistant  
*2026-08-25 16:31:23.484 UTC*

I'll create the doc with those three decisions applied.

**🔧 Tool call — `Bash`**

*Check format config and docs dir*

```bash
cat .clang-format; ls docs 2>/dev/null || echo "(no docs/ yet)"
```

<details>
<summary>Result</summary>

```
---
BasedOnStyle: LLVM
AccessModifierOffset: -4
ColumnLimit: 120
IndentWidth: 4
PointerAlignment: Left
AlwaysBreakTemplateDeclarations: true
BinPackArguments: false
BinPackParameters: OnePerLine
(no docs/ yet)
```

</details>

**🔧 Tool call — `Bash`**

*Write the render pipeline redesign doc*

````bash
mkdir -p docs && cat > docs/render_pipeline_redesign.md <<'ENDOFDOC'
# Render Pipeline Redesign

**Status:** Proposed — not yet implemented
**Date:** 2026-08-25
**Applies to:** the whole application; the rendering path in particular

This document proposes a decomposition that separates the frame loop, the simulation, the
rendering, and the Qt shell into modules with explicit contracts. It exists because the
current structure has no seam for anything that is neither "the simulation" nor "a widget" —
performance monitoring being the concrete case that exposed it.

Read `README.md` and `AGENTS.md` first. This document does not restate the coding rules; it
assumes them.

---

## 1. Why change

The current decomposition is sound at its one enforced boundary and thin everywhere else.

**What already works and must be preserved:**

- `fire_simulation` does not link Qt, so the simulation boundary is enforced by the build
  rather than by discipline. This is the single most valuable property in the codebase.
- `FireParameters` clamps on write and publishes its own bounds, so the UI derives slider
  ranges from the model instead of duplicating them.
- Data flow is one-way: intent travels down, frames travel up. No view reaches into the
  simulation.

**What blocks extension:**

| # | Problem | Where |
|---|---------|-------|
| 1 | `frameReady()` carries no payload, so nothing about a frame is observable | `src/app/FireController.hpp` |
| 2 | The catch-up clamp silently discards time — the one event a monitor most needs | `src/app/FireController.cpp` (accumulator clamp) |
| 3 | `FireWidget` is three classes in one: palette, heat-to-pixel conversion, painting | `src/app/FireWidget.hpp` |
| 4 | Frame geometry travels separately from frame data; mismatch is a silent dropped frame in release builds | `src/app/MainWindow.cpp`, `src/app/FireWidget.cpp` |
| 5 | `FireController` mixes timing, simulation ownership, and a per-parameter forwarding facade | `src/app/FireController.cpp` |
| 6 | Parameter binding is one-way; sliders go stale if anything else writes parameters | `src/app/ControlPanel.cpp` |
| 7 | Layering is directory convention, not a build constraint | `CMakeLists.txt` |
| 8 | No tests and no test target, despite a pure deterministic simulation | `CMakeLists.txt` |

Problem 1 is the root cause of problem 2 and of the difficulty in problems 3 and 5: with no
observable frame events, any component that needs facts about a frame must be injected into
the producer rather than subscribing to it.

---

## 2. Target layering

```
+-- app/ ------------------- Qt lives only here ---------------------+
|   MainWindow . FireController . ControlPanel . FireView            |
|   StatsPanel                                                       |
+---------------------------------+----------------------------------+
                                  | drives
+-- engine/ ----------------------v-- pure C++: the frame loop ------+
|   FireEngine . FrameClock . FrameReport                            |
+-----------+----------------------------------+---------------------+
            |                                  |
+-- sim/ ---v------ pure C++ ------+  +-- render/ v---- pure C++ ----+
|  FireSimulation . FireParameters |  |  FireRenderer . FirePalette  |
|  HeatFrame                       |  |  PixelBuffer  . Viewport     |
+----------------------------------+  +------------------------------+

+-- metrics/ ------- pure C++, no dependencies ----------------------+
|   TimeSeriesMetric . CounterMetric . ScopedTimer                   |
|   MetricHandle . FrameProfiler                                     |
+--------------------------------------------------------------------+
```

**Dependency rule:** `app -> engine -> {sim, render} -> metrics`, and `render -> sim`
(the renderer consumes `HeatFrame`). Only the executable links Qt.

CMake targets:

| Target | Links | Qt |
|--------|-------|----|
| `fire_metrics` | — | no |
| `fire_simulation` | — | no |
| `fire_render` | `fire_simulation` | no |
| `fire_engine` | `fire_simulation`, `fire_render`, `fire_metrics` | no |
| `classic_fire` | `fire_engine`, `fire_metrics`, `Qt::Widgets` | yes |

`fire_render` deliberately does not link `fire_metrics`: shading is timed by the engine at
the call site, so the renderer stays free of instrumentation concerns.

### Enforcement, stated honestly

`src` remains the single include root, so include spellings stay as they are
(`#include "sim/HeatFrame.hpp"`). Layering is therefore enforced by **link dependencies**,
not at compile time: a target that does not link `fire_render` will fail to link if it uses
it, but a header-only misuse would not be caught. This is an accepted tradeoff. Per-module
include roots would give compile-time enforcement at the cost of flattening every include
path to a bare filename.

---

## 3. The frame, end to end

```
QTimer wake (approximately 16 ms)
  |
  v FireController::onWake()
    elapsed = Clock::now() - lastWake
  |
  v FireEngine::advance(elapsed) ---------------------> FrameReport
      |
      +-- FrameClock::consume(elapsed) -> TickPlan{ticks, discardedTime}
      |
      +-- [Simulate]  N x FireSimulation::tick()          <-- ScopedTimer
      |
      +-- [Extract]   HeatFrame frame = simulation.heat()
      |
      +-- [Shade]     FireRenderer::render(frame)         <-- ScopedTimer
  |
  v emit frameReady(FrameReport)
      |
      +--> FireView::present(engine.frame())
      |      wrap PixelBuffer as QImage (no copy) -> update()
      |      paintEvent: fillRect + drawImage(fitPreservingAspect(...))  <-- ScopedTimer
      |
      +--> StatsPanel::update(engine.profiler().snapshot())
```

Three timed stages: **Simulate**, **Shade**, **Present**. Extract is a view construction and
costs nothing worth measuring.

All timing lives at two places: `FireEngine::advance` (Simulate, Shade) and
`FireView::paintEvent` (Present). Instrumentation belongs at the orchestration point, and
`FireEngine::advance` is that point.

---

## 4. Module reference

### 4.1 `sim/` — one addition

`FireSimulation` and `FireParameters` keep their current responsibilities. One new type:

```cpp
// Non-owning view of one simulation frame: the heat buffer together with the geometry that
// gives it meaning. Constructing one establishes cells.size() == width * height, so every
// consumer receives a correctly shaped buffer by construction.
class HeatFrame final {
public:
    constexpr HeatFrame(std::span<const std::uint8_t> cells, std::size_t width, std::size_t height) noexcept;

    [[nodiscard]] constexpr std::size_t width() const noexcept;
    [[nodiscard]] constexpr std::size_t height() const noexcept;
    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept;
    [[nodiscard]] constexpr std::span<const std::uint8_t> row(std::size_t y) const noexcept;
};
```

`FireSimulation::heat()` returns `HeatFrame` instead of a bare span. This is the render-input
contract, and it resolves problem 4: geometry can no longer drift from the data it describes,
and no consumer re-checks the agreement.

### 4.2 `metrics/` — new, Qt-free

```cpp
struct MetricStatistics {
    double averageMilliseconds{0.0};
    double percentile95Milliseconds{0.0};
    double maximumMilliseconds{0.0};
    std::size_t sampleCount{0};
};

// A rolling window of durations over the last MAXIMUM_SAMPLE_COUNT records. It knows nothing
// about what it measures: callers name a channel by choosing which instance to write to.
class TimeSeriesMetric final {
public:
    using Clock = std::chrono::steady_clock;

    void record(Clock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;
};

// A monotonic tally. What is being counted, and why, belongs to the caller.
class CounterMetric final {
public:
    void add(std::uint64_t amount = 1) noexcept;
    void clear() noexcept;

    [[nodiscard]] std::uint64_t value() const noexcept;
};

// Times the enclosing scope and records on destruction. Declaring one before other scoped
// resources measures their teardown too, because they are destroyed first.
class ScopedTimer final {
public:
    explicit ScopedTimer(TimeSeriesMetric& metric) noexcept;
    ~ScopedTimer() noexcept;

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

// Opaque, stable reference to one registered channel. Resolved once at registration; the
// per-frame path is an array index, not a lookup by name.
class MetricHandle final { /* strong index type */ };

struct ProfileSnapshot {
    struct Channel {
        std::string_view name;
        MetricStatistics statistics;
    };
    std::vector<Channel> channels;
};

// Owns a registered set of named timing channels. Channels are registered, never hard-coded,
// so readers iterate a snapshot and stay unchanged as stages come and go.
class FrameProfiler final {
public:
    // Precondition: name must have static storage duration (a string literal). The profiler
    // stores the view, not a copy.
    [[nodiscard]] MetricHandle registerChannel(std::string_view name);

    [[nodiscard]] TimeSeriesMetric& channel(MetricHandle handle) noexcept;
    [[nodiscard]] const TimeSeriesMetric& channel(MetricHandle handle) const noexcept;

    [[nodiscard]] ProfileSnapshot snapshot() const;
    void clear() noexcept;
};
```

`TimeSeriesMetric` uses a fixed-size ring buffer, so recording never allocates.

**Why registration rather than named members.** A profiler with `TimeSeriesMetric simulate;
shade; present;` forces three coordinated edits — profiler, snapshot struct, and panel rows —
every time a stage is added or split. With registration the profiler never learns stage names
at compile time and `StatsPanel` iterates the snapshot, so a new channel produces a new row
with no UI change. Lookup by pre-resolved index rather than by string keeps measurement out
of the measurement.

### 4.3 `render/` — new, Qt-free

Four types, no virtual functions, no per-frame allocation after the first frame.

```cpp
// render/PixelBuffer.hpp
using Rgba32 = std::uint32_t;   // 0xFFRRGGBB, byte-identical to QImage::Format_RGB32

// Owns the destination pixels together with their geometry — the output-side counterpart to
// HeatFrame.
class PixelBuffer final {
public:
    void resize(std::size_t width, std::size_t height);

    [[nodiscard]] std::size_t width() const noexcept;
    [[nodiscard]] std::size_t height() const noexcept;
    [[nodiscard]] std::span<Rgba32> row(std::size_t y) noexcept;
    [[nodiscard]] const Rgba32* data() const noexcept;
};

// render/FirePalette.hpp

// The 256-entry colour lookup table: a transfer function from heat to colour. Value
// semantics, so a palette can be copied, compared and swapped without touching the renderer.
class FirePalette final {
public:
    static FirePalette classic();
    static FirePalette fromStops(std::span<const PaletteStop> stops);

    [[nodiscard]] Rgba32 operator[](std::uint8_t heat) const noexcept;
};

// render/Viewport.hpp

struct FitRect {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
};

// Largest centred rectangle of the source aspect ratio that fits the available area.
[[nodiscard]] constexpr FitRect fitPreservingAspect(int availableWidth,
                                                    int availableHeight,
                                                    int sourceWidth,
                                                    int sourceHeight) noexcept;

// render/FireRenderer.hpp

// Converts a simulation frame into displayable pixels. Owns its destination buffer and sizes
// it from the frame it is handed, so frame geometry is never configured separately from the
// data it describes.
class FireRenderer final {
public:
    explicit FireRenderer(FirePalette palette) noexcept;

    void setPalette(const FirePalette& palette) noexcept;

    // Reallocates the destination buffer when the frame geometry changes.
    void render(const HeatFrame& heat);

    [[nodiscard]] const PixelBuffer& target() const noexcept;
};
```

`render()` is a pure function with a cache: resize if geometry changed, then map each heat
byte through the palette. It is fully testable without Qt.

`Viewport` holds the letterbox arithmetic currently buried in `FireWidget::fittedFrameRect`.
It returns a plain `FitRect` that the Qt layer converts to a `QRect`, which keeps `render/`
free of Qt for the price of three lines in the view.

### 4.4 `engine/` — new, Qt-free

```cpp
// engine/FrameClock.hpp

struct TickPlan {
    int ticks{0};
    std::chrono::steady_clock::duration discardedTime{};   // non-zero: could not keep up
};

// Fixed-timestep accumulator, isolated from Qt and from the simulation so it can be driven
// with synthetic elapsed times in tests.
class FrameClock final {
public:
    FrameClock(int ticksPerSecond, int maximumTicksPerWake);

    [[nodiscard]] TickPlan consume(std::chrono::steady_clock::duration elapsed) noexcept;
    void reset() noexcept;
};
```

`TickPlan::discardedTime` is the fix for problem 2: the catch-up clamp becomes a returned
fact instead of a silent local adjustment.

```cpp
// engine/FrameReport.hpp

struct FrameReport {
    int ticksExecuted{0};
    std::chrono::steady_clock::duration discardedTime{};
    std::uint64_t frameIndex{0};
};

// engine/FireEngine.hpp

// The frame loop and the owner of every long-lived pure-C++ resource. This is where the
// profiler lives, so no measurement concern is threaded through Qt constructors.
class FireEngine final {
public:
    FireEngine(std::size_t simulationWidth, std::size_t simulationHeight);

    FrameReport advance(std::chrono::steady_clock::duration elapsed);
    void reset() noexcept;

    [[nodiscard]] const PixelBuffer& frame() const noexcept;

    [[nodiscard]] const FireParameters& parameters() const noexcept;
    void setParameters(const FireParameters& parameters) noexcept;

    [[nodiscard]] FrameProfiler& profiler() noexcept;
    [[nodiscard]] const FrameProfiler& profiler() const noexcept;

private:
    FireSimulation simulation;
    FireRenderer renderer;
    FrameClock clock;
    FrameProfiler frameProfiler;
    MetricHandle simulateChannel;
    MetricHandle shadeChannel;
    std::uint64_t frameIndex{0};
};
```

`advance` is the whole loop:

```cpp
FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) {
    const TickPlan plan = clock.consume(elapsed);

    {
        const ScopedTimer timer{frameProfiler.channel(simulateChannel)};
        for (int tick = 0; tick < plan.ticks; ++tick) {
            simulation.tick();
        }
    }
    {
        const ScopedTimer timer{frameProfiler.channel(shadeChannel)};
        renderer.render(simulation.heat());
    }

    return FrameReport{plan.ticks, plan.discardedTime, ++frameIndex};
}
```

`FireEngine` is testable headlessly: advance it sixty times with synthetic elapsed values and
assert on `frame()` and the reports.

`setParameters` taking a whole `FireParameters` value — rather than one setter per field —
resolves problems 5 and 6 together: two engine entry points regardless of knob count, and the
panel round-trips a value it can also read back.

### 4.5 `app/` — Qt shell

| Class | Responsibility |
|-------|----------------|
| `FireController` | Qt adapter only. Owns `QTimer` and `FireEngine`; run/pause/toggle/reset; emits `frameReady(FrameReport)` and `runningChanged(bool)`. |
| `FireView` (was `FireWidget`) | Presentation surface only. `present(const PixelBuffer&)` wraps and calls `update()`; `paintEvent` blits into `fitPreservingAspect(...)`. No palette, no conversion, no simulation constants. |
| `ControlPanel` | Binds sliders to a `FireParameters` value; emits `parametersChanged(FireParameters)`. Two-way, so it cannot go stale. |
| `StatsPanel` | Renders a `ProfileSnapshot` generically, one row per channel. |
| `MainWindow` | Wiring only. Holds no simulation dimensions — the view derives geometry from the buffers it receives. |

`FireView` takes a single `TimeSeriesMetric&` for the Present channel. This is the only place
a metrics type crosses into a widget constructor, and it is deliberately the narrowest
possible dependency: "record durations here", not "here is a monitor that knows every stage".

`FireParameters` is passed by value through a signal on a direct, same-thread connection, so
no `qRegisterMetaType` is required. If that connection ever becomes queued, it will be.

---

## 5. Contracts and constraints

**Qt boundary.** `sim`, `render`, `engine` and `metrics` must never include a Qt header. Only
`classic_fire` links Qt. This extends the existing rule from `AGENTS.md` to the whole spine.

**Frame geometry.** `HeatFrame` carries the simulation geometry; `PixelBuffer` carries the
target geometry. No third party configures either. `SIMULATION_WIDTH`/`SIMULATION_HEIGHT`
exist at exactly one site — the composition root — and are handed only to `FireEngine`.

**Pixel format.** `Rgba32` is `0xFFRRGGBB` in native byte order, matching
`QImage::Format_RGB32`. `FireView` wraps the buffer with the non-owning
`QImage(const uchar*, int, int, Format)` constructor and never copies.

**Lifetime.** `FireView` holds a `QImage` that borrows the engine's `PixelBuffer`, so the
engine must outlive the widget subtree. `FireController` owns the engine, so the composition
root must construct the central widget *before* the controller: Qt destroys children in list
order, so the widget subtree is then torn down first. If this proves too subtle to rely on,
`QImage::copy()` at present time removes the constraint for the cost of one copy per frame.

**Single-threaded rendering.** `QWidget::update()` posts a deferred paint request, so
`paintEvent` never runs nested inside the timer slot, and `FireEngine::advance` spins no
nested event loop. Rendering and painting are therefore strictly serialized by the event loop
and a single buffer is safe. This assumption is why there is no double buffering; it holds
only while nothing calls synchronous `repaint()` from the render path.

---

## 6. Design decisions

Recorded so they are not silently re-litigated.

### 6.1 No `RenderPass` / `RenderContext` / `RenderPipeline` — rejected

A pass interface with a linear pipeline was considered and rejected. Its main argument was
that self-identifying stages let the pipeline register profiler channels automatically, so
instrumentation is not hand-written at each site. That argument dissolves once `FireEngine`
exists: the engine is a single orchestration point that runs every stage, so two explicit
`ScopedTimer` scopes in one function give the same property without a vtable, an owned
`vector<unique_ptr<RenderPass>>`, or a per-frame context struct.

Runtime pass ordering and polymorphic extension were the other arguments, and both are
speculative — there is exactly one pass.

**Cost of the decision:** adding a post-process effect later means editing
`FireRenderer::render` rather than adding one registration line; per-effect timing needs a
second `ScopedTimer` inside `render()`.

**Why it is safe:** nothing outside `render/` can observe the difference. `FireEngine` calls
`renderer.render(heat)` and `FireView` receives a `const PixelBuffer&` under either design,
so the abstraction can be reintroduced as a `render/`-internal refactor if a second effect
ever justifies it.

### 6.2 No double buffering — rejected

See the single-threaded rendering constraint in section 5. The event loop already serializes
writes and reads, so a second buffer protects against nothing. It would be required if
rendering ever moved off the UI thread; that change would be internal to `FireRenderer`,
since `PixelBuffer` is already the unit that would be doubled.

### 6.3 No `RenderSettings` — deferred

A render-side settings value mirroring `FireParameters` was considered. It is deferred
because there is currently exactly one plausible render knob (palette choice), which
`FireRenderer::setPalette` covers, and a second candidate (smooth scaling) belongs to the
view rather than the renderer. Introduce the type when a second render knob actually exists.

This does not weaken the fix for problem 6, which concerns simulation parameters and is
resolved by round-tripping `FireParameters`.

### 6.4 No interpolation alpha — dropped

An `interpolationAlpha` on `TickPlan` was considered so the renderer could blend between
simulation states. With the simplified renderer it has no consumer, and an unused field is
dead weight. `FrameClock` can compute it in one line when something needs it.

### 6.5 `render/` is Qt-free rather than QImage-based — accepted with a cost

Keeping the renderer free of the windowing system is the load-bearing separation in
real-time graphics and it makes the entire pixel path unit-testable. The cost is the wrapping
step at the Qt boundary and the lifetime constraint in section 5. Letting `FireRenderer` own
a `QImage` directly would be a defensible simplification that trades that testability away.

---

## 7. Migration plan

Each step builds, ships, and leaves the application working.

1. **`fire_metrics` plus a test target.** Pure addition, no callers. Establishes CTest, which
   is what makes the later steps safe. Resolves problem 8 for this module.
2. **`HeatFrame`; `fire_render` with `PixelBuffer`, `FirePalette`, `FireRenderer`,
   `Viewport`; tests.** `FireWidget` loses its palette and conversion code and becomes
   `FireView`. Resolves problems 3 and 4.
3. **`FrameClock` plus tests.** `FireController` loses the accumulator. Resolves problem 2.
4. **`FireEngine`.** `FireController` loses everything else and becomes a Qt adapter.
   Resolves problems 1 and 5.
5. **`ControlPanel` binds a `FireParameters` value.** Resolves problem 6.
6. **`StatsPanel`.** The feature that motivated the redesign; by this point it is additive.

Step 1 comes first specifically because it is the only pure addition and it delivers the test
target that steps 2 through 4 depend on.

Problem 7 is addressed incrementally as targets are split in steps 1, 2 and 4, to the extent
described in section 2.

---

## 8. Out of scope

Named explicitly so they are not built speculatively:

- **Threaded rendering.** Would require reinstating double buffering inside `FireRenderer`.
- **Frame interpolation.** See decision 6.4.
- **Post-process effects** (glow, bloom, scanlines). See decision 6.1.
- **A general frame graph** with declared resources and dependency sorting.
- **A GPU path.** Worth knowing that `PixelBuffer` and `FirePalette` map onto a texture
  upload and a LUT sampler if `QOpenGLWidget` is ever wanted, but nothing here should be
  shaped around that today.

---

## 9. Open questions

- Whether `StatsPanel` should also display counters (total ticks, frames that discarded
  time) alongside timing channels. `FrameProfiler` would need a counter registration path
  symmetric to `registerChannel`.
- Whether `Viewport` earns its own translation unit or should stay a private helper on
  `FireView`. It is extracted here for testability; the arithmetic is small.
- Whether the lifetime constraint in section 5 is acceptable, or whether the per-frame
  `QImage::copy()` is the better trade.
ENDOFDOC
wc -l docs/render_pipeline_redesign.md
````

<details>
<summary>Result</summary>

```
     550 docs/render_pipeline_redesign.md
```

</details>

**🔧 Tool call — `Bash`**

*Spot-check the written doc*

````bash
sed -n '1,20p;95,115p;300,320p' docs/render_pipeline_redesign.md && echo "...OK..." && grep -c '```' docs/render_pipeline_redesign.md && git status --short
````

<details>
<summary>Result</summary>

````
# Render Pipeline Redesign

**Status:** Proposed — not yet implemented
**Date:** 2026-08-25
**Applies to:** the whole application; the rendering path in particular

This document proposes a decomposition that separates the frame loop, the simulation, the
rendering, and the Qt shell into modules with explicit contracts. It exists because the
current structure has no seam for anything that is neither "the simulation" nor "a widget" —
performance monitoring being the concrete case that exposed it.

Read `README.md` and `AGENTS.md` first. This document does not restate the coding rules; it
assumes them.

---

## 1. Why change

The current decomposition is sound at its one enforced boundary and thin everywhere else.

path to a bare filename.

---

## 3. The frame, end to end

```
QTimer wake (approximately 16 ms)
  |
  v FireController::onWake()
    elapsed = Clock::now() - lastWake
  |
  v FireEngine::advance(elapsed) ---------------------> FrameReport
      |
      +-- FrameClock::consume(elapsed) -> TickPlan{ticks, discardedTime}
      |
      +-- [Simulate]  N x FireSimulation::tick()          <-- ScopedTimer
      |
      +-- [Extract]   HeatFrame frame = simulation.heat()
      |
      +-- [Shade]     FireRenderer::render(frame)         <-- ScopedTimer
    [[nodiscard]] const PixelBuffer& target() const noexcept;
};
```

`render()` is a pure function with a cache: resize if geometry changed, then map each heat
byte through the palette. It is fully testable without Qt.

`Viewport` holds the letterbox arithmetic currently buried in `FireWidget::fittedFrameRect`.
It returns a plain `FitRect` that the Qt layer converts to a `QRect`, which keeps `render/`
free of Qt for the price of three lines in the view.

### 4.4 `engine/` — new, Qt-free

```cpp
// engine/FrameClock.hpp

struct TickPlan {
    int ticks{0};
    std::chrono::steady_clock::duration discardedTime{};   // non-zero: could not keep up
};

...OK...
16
?? docs/
````

</details>

Written to [docs/render_pipeline_redesign.md](docs/render_pipeline_redesign.md) — 550 lines, untracked (I didn't commit).

**Structure:**

| § | Contents |
|---|---|
| 1 | Why change — what to preserve, plus the 8 numbered problems, referenced by number throughout |
| 2 | Target layering, CMake targets, and an honest note that layering is enforced by *link* deps only |
| 3 | Frame flow end to end, three timed stages |
| 4 | Module reference — `sim/`, `metrics/`, `render/`, `engine/`, `app/` with API sketches |
| 5 | Contracts: Qt boundary, geometry, pixel format, lifetime, single-threaded rendering |
| 6 | Design decisions with rationale for each rejection |
| 7 | Migration plan, 6 steps, each shippable |
| 8 | Out of scope |
| 9 | Open questions |

**Your three decisions are applied:** `FireRenderer` + `PixelBuffer` + `FirePalette` + `Viewport`, no pass/context/pipeline, no double buffering; `RenderSettings` deferred with the reasoning recorded; `FrameProfiler` kept with handle-based registration, including the argument for registration over named members.

**One judgment call I made beyond what you specified:** I dropped `interpolationAlpha` from `TickPlan` (decision 6.4). Once the pipeline went, it had no consumer — `render(heat)` takes no alpha — and an unused field is exactly the dead weight AGENTS.md warns against. `FrameClock` can compute it in one line if something ever needs it. Easy to reverse if you'd rather keep the seam.

Section 6 is written as a decision log on purpose: it records *why* the pass abstraction was rejected and under what condition it should come back. Without that, the next contributor reading `FireRenderer::render` has a good chance of "helpfully" reintroducing it.

Two things I did **not** touch, since you scoped this to the one file:

- The doc map table in [AGENTS.md:21](AGENTS.md:21) has one row (`Anything → README.md`). A row like `Rendering, frame loop, or metrics → docs/render_pipeline_redesign.md` would wire this into the mechanism the repo already has for pointing contributors at area docs. One line, say the word.
- Nothing in `src/`.

## 👤 User  
*2026-08-25 18:02:07.339 UTC*

Address these review concerns:

Verdict
It's a good document — genuinely well-reasoned, honest about its tradeoffs, and the migration plan is sequenced correctly. But it is not a rendering pipeline redesign. It's a modularity and observability redesign that leaves the rendering pipeline behaving exactly as it does today. That gap is the main thing I'd want addressed before implementing it.
The central problem
Not one of the eight entries in the problem table ([:34-41](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:34)) is "frames don't pace evenly." They're all structural: no payload on the signal, no seam, no test target, mixed responsibilities. Real problems, all of them. But after all six migration steps land, the app still wakes on a 16 ms `QTimer` against a 16.667 ms step, still shades on the simulation clock into an `update()` that coalesces, and still re-runs the bilinear upscale on every repaint. The judder is identical. You will simply have a panel that can show it to you.
That may be exactly the intent — the doc says performance monitoring is what exposed the missing seam — but then the title and framing oversell. Either rename it (`docs/architecture_redesign.md`) or add a ninth problem for frame pacing and say where it gets fixed.
The instrumentation as designed won't diagnose pacing either. The three timed stages are Simulate, Shade, Present — all durations. Smoothness is determined by intervals: the wall-clock gap between presented frames, and the distribution of ticks-per-presented-frame. `FrameReport` ([:338-342](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:338)) carries `ticksExecuted`, which is the raw material, but nothing records the wake interval or the present interval. You'll get a StatsPanel showing three stage times summing to 3 ms while the fire visibly stutters, because the stutter is in the 13 ms you aren't measuring. Add `elapsed` to `FrameReport` and a `TimeSeriesMetric` for the paint-to-paint interval; those two numbers are worth more than all three stage timers.
Substantive issues
`advance()` shades unconditionally ([:377-392](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:377)). Current code guards with `if (ticksExecuted > 0)`. The new loop runs `renderer.render()` even when `plan.ticks == 0`, which — given the 16/16.667 beat — is roughly 4% of wakes, converting unchanged heat into identical pixels and posting a redundant `update()`. That's a regression, and it also contaminates the Shade metric with work that shouldn't exist. Needs an explicit early return and a stated policy.
Push-shading gets locked in. Today the wasted conversion on coalesced updates is a wiring accident in `MainWindow`. After this redesign it's structural: shading happens inside the engine, on the timer clock, in the pure layer, where the view has no way to defer it. §5's observation that `update()` is deferred is used to justify skipping double buffering (correct), but the same fact means every shade whose `update()` gets coalesced is thrown away — and the doc doesn't notice. Worth considering `render(heat, PixelBuffer&)` with the view owning the buffer: renderer becomes stateless, shading can move to paint time, and — see below — the lifetime problem evaporates.
The lifetime constraint is the weakest part of the design ([:434-438](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:434)). "The composition root must construct the central widget before the controller, because Qt destroys children in list order" is load-bearing correctness resting on an ordering nobody will remember. You have ASan builds configured; this is precisely what they'll catch, once, at shutdown, on someone else's machine. Open question 9.3 asks whether to take the `QImage::copy()` instead — at a few hundred KB per frame that copy costs ~15 µs and buys you an invariant you can't accidentally break. Take it, or invert buffer ownership as above. Don't ship the ordering rule.
`metrics/` is over-built by the doc's own standard. §6.1 rejects `RenderPass` because "there is exactly one pass" and runtime extensibility is speculative. That's the right call. But the registration machinery — `MetricHandle` as a strong index, `registerChannel`, `ProfileSnapshot` with a `vector<Channel>`, a generic row-iterating panel — is justified at [:232-237](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:232) with the identical speculative-extension argument: that adding a stage later would need three coordinated edits. There are three channels. They're enumerated in the document. Three named `TimeSeriesMetric` members and three labelled rows costs less and reads better, and the doc has no principled reason to accept the abstraction here after rejecting it there.
Related: `snapshot()` allocates a vector, and §3 calls it every frame — against the file's own "recording never allocates" discipline. `CounterMetric` has no consumer (9.1 asks whether to invent one), so by §6's standard it shouldn't exist yet. And `registerChannel`'s "name must have static storage duration" ([:220](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:220)) is an unenforceable precondition living in a comment — take `const char*` at minimum.
Two-way parameter binding has no mechanism ([:408](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:408)). "Two-way, so it cannot go stale" — but the doc only specifies the panel→engine direction (`parametersChanged`). Nothing is described that pushes engine state back into the sliders, and the obvious implementation loops while a slider is being dragged. Either specify the signal and how the cycle is broken, or drop the claim and keep it one-way with an explicit refresh on reset.
`FireSimulation` still has no test. Problem 8 calls out "a pure deterministic simulation" with no tests, but the migration adds tests for metrics, render, and `FrameClock` only. The fixed seed exists precisely so you can hash the heat map after N ticks and assert on it. That test is cheap and it's the one problem 8 actually named.
Step 2 is too big ([:511](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:511)). A new target, four new types, a signature change on `FireSimulation::heat()`, and gutting/renaming the widget — in a step that's meant to "build, ship, and leave the application working." Split it: `HeatFrame` and the `heat()` change; then `render/` plus tests; then `FireWidget` → `FireView`.
Inconsistencies worth a quick pass

* §2's dependency rule says `{sim, render} -> metrics` ([:72](https://claude.ai/epitaxy/docs/render_pipeline_redesign.md:72)), but the target table immediately below shows neither linking `fire_metrics`, and the following paragraph says that's deliberate.
* §4.5 says `MainWindow` "Holds no simulation dimensions"; §5 says the dimensions live at "exactly one site — the composition root." `MainWindow` is the composition root.
* `FirePalette::fromStops` takes `std::span<const PaletteStop>`; `PaletteStop` is never declared.
* §3 has `StatsPanel::update(...)` on every frame — 60 Hz label updates will be both wasteful and unreadable. Throttle to ~4 Hz.

What I'd actually do
Keep `HeatFrame`, `FrameClock`, `FireEngine`, `setParameters`-by-value, the Qt-free `render/` layer, the rejection of `RenderPass`, and the migration ordering — all of that is right. Cut the metrics registration layer down to three named channels. Fix `advance()` to skip shading on zero ticks. Replace the destruction-order rule with the copy. Add `elapsed` to `FrameReport` and a present-interval metric, so the panel measures the thing the redesign is named after. And decide explicitly, in writing, whether frame pacing is in scope — right now it's neither fixed nor listed in §8's out-of-scope, which is the only place the document is silent where it should be loud.

## 🤖 Assistant  
*2026-08-25 18:05:12.774 UTC*

Working through the review. Most of it lands; several points change the design rather than just the prose. Let me rewrite the doc.

**🔧 Tool call — `Bash`**

*Rewrite doc addressing review*

````bash
cat > docs/render_pipeline_redesign.md <<'ENDOFDOC'
# Render Pipeline Redesign

**Status:** Proposed — not yet implemented
**Date:** 2026-08-25
**Applies to:** the whole application; the rendering path in particular

## Scope, stated up front

This is an **architecture redesign centred on the render path**, not a rewrite of how frames
are produced. Most of what follows is about modularity and observability: giving the frame
loop, the simulation, the rendering, and the Qt shell explicit contracts and separate
lifetimes. Performance monitoring is the concrete feature that exposed the missing seam.

Frame pacing is treated as a first-class problem (problem 9) rather than left implicit, but
it is only **partially** fixed here. Section 6.7 states exactly which part, and section 8
names the part that stays out of scope. If you are reading this hoping for smooth
vsync-aligned presentation, read 6.7 first — you will not get it from these six steps.

Read `README.md` and `AGENTS.md` first. This document does not restate the coding rules; it
assumes them.

---

## 1. Why change

The current decomposition is sound at its one enforced boundary and thin everywhere else.

**What already works and must be preserved:**

- `fire_simulation` does not link Qt, so the simulation boundary is enforced by the build
  rather than by discipline. This is the single most valuable property in the codebase.
- `FireParameters` clamps on write and publishes its own bounds, so the UI derives slider
  ranges from the model instead of duplicating them.
- Data flow is one-way: intent travels down, frames travel up. No view reaches into the
  simulation.

**What blocks extension:**

| # | Problem | Where |
|---|---------|-------|
| 1 | `frameReady()` carries no payload, so nothing about a frame is observable | `src/app/FireController.hpp` |
| 2 | The catch-up clamp silently discards time — the one event a monitor most needs | `src/app/FireController.cpp` (accumulator clamp) |
| 3 | `FireWidget` is three classes in one: palette, heat-to-pixel conversion, painting | `src/app/FireWidget.hpp` |
| 4 | Frame geometry travels separately from frame data; mismatch is a silent dropped frame in release builds | `src/app/MainWindow.cpp`, `src/app/FireWidget.cpp` |
| 5 | `FireController` mixes timing, simulation ownership, and a per-parameter forwarding facade | `src/app/FireController.cpp` |
| 6 | Parameter binding is one-way; sliders would go stale if anything else wrote parameters | `src/app/ControlPanel.cpp` |
| 7 | Layering is directory convention, not a build constraint | `CMakeLists.txt` |
| 8 | No tests and no test target, despite a pure deterministic simulation | `CMakeLists.txt` |
| 9 | The wake interval is *longer* than the simulation step, so the accumulator starves on a fixed beat and ticks arrive unevenly | `src/app/FireController.hpp` |

Problem 1 is the root cause of problem 2 and of the difficulty in problems 3 and 5: with no
observable frame events, any component that needs facts about a frame must be injected into
the producer rather than subscribing to it.

**Problem 9 in numbers.** `WAKE_INTERVAL_MILLISECONDS` is 16; the simulation step is
1/60 s = 16.667 ms. Each wake therefore delivers less time than one tick consumes. Over one
second, 62.5 wakes deliver 1000 ms and produce 60 ticks — so roughly 2.5 wakes per second,
about 4% of them, produce **zero** ticks and no new frame. That is a systematic beat, not
jitter, and it is present in every build. It is listed last because it is independent of
problems 1 through 8, but it is the one a viewer actually sees.

---

## 2. Target layering

```
+-- app/ ------------------- Qt lives only here ---------------------+
|   MainWindow . FireController . ControlPanel . FireView            |
|   StatsPanel                                                       |
+---------------------------------+----------------------------------+
                                  | drives
+-- engine/ ----------------------v-- pure C++: the frame loop ------+
|   FireEngine . FrameClock . FrameReport                            |
+-----------+---------------------------------------------------------+
            |
+-- sim/ ---v------ pure C++ ------+  +-- render/ ---- pure C++ ------+
|  FireSimulation . FireParameters |  |  FireRenderer . FirePalette   |
|  HeatFrame                       |<-|  PixelBuffer  . Viewport      |
+----------------------------------+  +-------------------------------+

+-- metrics/ ------- pure C++, no dependencies ----------------------+
|   MetricStatistics . TimeSeriesMetric . ScopedTimer                |
+--------------------------------------------------------------------+
```

**Dependency rule:**

- `app` depends on `engine`, `render` and `metrics`
- `engine` depends on `sim` and `metrics`
- `render` depends on `sim` (it consumes `HeatFrame`)
- `sim` and `render` depend on nothing else — in particular **not** on `metrics`

Only the executable links Qt.

CMake targets:

| Target | Links | Qt |
|--------|-------|----|
| `fire_metrics` | — | no |
| `fire_simulation` | — | no |
| `fire_render` | `fire_simulation` | no |
| `fire_engine` | `fire_simulation`, `fire_metrics` | no |
| `classic_fire` | `fire_engine`, `fire_render`, `fire_metrics`, `Qt::Widgets` | yes |

Neither `fire_simulation` nor `fire_render` links `fire_metrics`: each is timed by its
caller, so neither carries an instrumentation dependency. `fire_engine` does not link
`fire_render` either — rendering is driven by the view, for the reasons in section 6.5.

### Enforcement, stated honestly

`src` remains the single include root, so include spellings stay as they are
(`#include "sim/HeatFrame.hpp"`). Layering is therefore enforced by **link dependencies**,
not at compile time: a target that does not link `fire_render` will fail to link if it uses
it, but a header-only misuse would not be caught. This is an accepted tradeoff. Per-module
include roots would give compile-time enforcement at the cost of flattening every include
path to a bare filename.

---

## 3. The frame, end to end

```
QTimer wake (8 ms — see section 6.7)
  |
  v FireController::onWake()
    elapsed = Clock::now() - lastWake          --> records WakeInterval
  |
  v FireEngine::advance(elapsed) ---------------------> FrameReport
      |
      +-- FrameClock::consume(elapsed) -> TickPlan{ticks, discardedTime}
      |
      +-- if plan.ticks == 0: return early, no frame produced
      |
      +-- [Simulate]  N x FireSimulation::tick()          <-- ScopedTimer
  |
  v if report.producedFrame():  emit frameReady(FrameReport)
      |
      +--> FireView::present(engine.heat())
             [Shade]   FireRenderer::render(heat, ownBuffer)   <-- ScopedTimer
             update()
             ...later, own event-loop iteration...
             paintEvent:  records PresentInterval (paint-to-paint gap)
                          [Present] fillRect + drawImage(fit)  <-- ScopedTimer

MainWindow refresh timer (250 ms — see section 4.5)
  |
  v StatsPanel::update(assembled FrameStatistics)   <-- pulls, never holds references
```

Five measured channels, three durations and two intervals:

| Channel | Kind | Recorded by | Answers |
|---------|------|-------------|---------|
| `Simulate` | duration | `FireEngine::advance` | how long ticking costs |
| `Shade` | duration | `FireView::present` | how long heat-to-pixels costs |
| `Present` | duration | `FireView::paintEvent` | how long the blit and upscale cost |
| `WakeInterval` | interval | `FireController::onWake` | is the timer delivering on time |
| `PresentInterval` | interval | `FireView::paintEvent` | **do frames actually land evenly** |

The two intervals are the point. Stage durations answer "where does the time go"; only
intervals answer "is it smooth". A build can show three stage timings summing to 3 ms while
the fire visibly stutters, because the stutter lives in the 13 ms between them. `WakeInterval`
and `PresentInterval` are the two numbers that make problem 9 visible.

`FrameReport::ticksExecuted` divided by frames presented gives ticks-per-presented-frame,
which distinguishes "the timer is late" from "the simulation is slow".

---

## 4. Module reference

### 4.1 `sim/` — one addition

`FireSimulation` and `FireParameters` keep their current responsibilities. One new type:

```cpp
// Non-owning view of one simulation frame: the heat buffer together with the geometry that
// gives it meaning. Constructing one establishes cells.size() == width * height, so every
// consumer receives a correctly shaped buffer by construction.
class HeatFrame final {
public:
    constexpr HeatFrame(std::span<const std::uint8_t> cells, std::size_t width, std::size_t height) noexcept;

    [[nodiscard]] constexpr std::size_t width() const noexcept;
    [[nodiscard]] constexpr std::size_t height() const noexcept;
    [[nodiscard]] constexpr std::span<const std::uint8_t> cells() const noexcept;
    [[nodiscard]] constexpr std::span<const std::uint8_t> row(std::size_t y) const noexcept;
};
```

`FireSimulation::heat()` returns `HeatFrame` instead of a bare span. This is the render-input
contract, and it resolves problem 4: geometry can no longer drift from the data it describes,
and no consumer re-checks the agreement.

`HeatFrame` is only ever passed as an argument and never stored, so it never outlives the
simulation that produced it.

### 4.2 `metrics/` — new, Qt-free, three types

```cpp
struct MetricStatistics {
    double averageMilliseconds{0.0};
    double percentile95Milliseconds{0.0};
    double maximumMilliseconds{0.0};
    std::size_t sampleCount{0};
};

// A rolling window over the last MAXIMUM_SAMPLE_COUNT durations, held in a fixed-size ring
// buffer so recording never allocates. It knows nothing about what it measures: callers name
// a channel by choosing which instance to write to.
class TimeSeriesMetric final {
public:
    using Clock = std::chrono::steady_clock;

    void record(Clock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;   // returns by value, no allocation
};

// Times the enclosing scope and records on destruction. Declaring one before other scoped
// resources measures their teardown too, because they are destroyed first.
class ScopedTimer final {
public:
    explicit ScopedTimer(TimeSeriesMetric& metric) noexcept;
    ~ScopedTimer() noexcept;

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};
```

That is the whole module. There is no profiler type, no channel registry, and no handle.

**Each component owns the metrics it produces**, exposed as `const` accessors:

```cpp
// FireEngine
[[nodiscard]] const TimeSeriesMetric& simulateTime() const noexcept;
[[nodiscard]] const TimeSeriesMetric& wakeInterval() const noexcept;

// FireView
[[nodiscard]] const TimeSeriesMetric& shadeTime() const noexcept;
[[nodiscard]] const TimeSeriesMetric& presentTime() const noexcept;
[[nodiscard]] const TimeSeriesMetric& presentInterval() const noexcept;
```

Nothing is shared, nothing is injected, and no object holds a reference to a metric owned by
another object. `MainWindow`'s refresh timer pulls `statistics()` from each producer by value
and pushes an assembled `FrameStatistics` into `StatsPanel`, so `StatsPanel` stores no
references either. See decision 6.6 for why this replaced a registration-based profiler.

### 4.3 `render/` — new, Qt-free

Four types, no virtual functions, no per-frame allocation after the first frame.

```cpp
// render/PixelBuffer.hpp
using Rgba32 = std::uint32_t;   // 0xFFRRGGBB, byte-identical to QImage::Format_RGB32

// Owns the destination pixels together with their geometry — the output-side counterpart to
// HeatFrame.
class PixelBuffer final {
public:
    void resize(std::size_t width, std::size_t height);

    [[nodiscard]] std::size_t width() const noexcept;
    [[nodiscard]] std::size_t height() const noexcept;
    [[nodiscard]] std::span<Rgba32> row(std::size_t y) noexcept;
    [[nodiscard]] const Rgba32* data() const noexcept;
};

// render/FirePalette.hpp

// One control point of the colour ramp.
struct PaletteStop {
    std::uint8_t heat;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

// The 256-entry colour lookup table: a transfer function from heat to colour. Value
// semantics, so a palette can be copied, compared and swapped without touching the renderer.
class FirePalette final {
public:
    static FirePalette classic();
    static FirePalette fromStops(std::span<const PaletteStop> stops);

    [[nodiscard]] Rgba32 operator[](std::uint8_t heat) const noexcept;
};

// render/Viewport.hpp

struct FitRect {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
};

// Largest centred rectangle of the source aspect ratio that fits the available area.
[[nodiscard]] constexpr FitRect fitPreservingAspect(int availableWidth,
                                                    int availableHeight,
                                                    int sourceWidth,
                                                    int sourceHeight) noexcept;

// render/FireRenderer.hpp

// Converts a simulation frame into displayable pixels. Stateless apart from the palette: the
// caller supplies the destination, and the renderer sizes it from the frame it is handed, so
// frame geometry is never configured separately from the data it describes.
class FireRenderer final {
public:
    explicit FireRenderer(FirePalette palette) noexcept;

    void setPalette(const FirePalette& palette) noexcept;

    // Resizes target when the frame geometry changes, then maps each heat byte through the
    // palette.
    void render(const HeatFrame& heat, PixelBuffer& target) const;
};
```

`render()` owns no buffer. That is what lets the destination live wherever it is most
convenient — in practice, inside the widget that paints it — and it is what removes the
cross-object lifetime constraint entirely (section 5).

`Viewport` holds the letterbox arithmetic currently buried in `FireWidget::fittedFrameRect`.
It returns a plain `FitRect` that the Qt layer converts to a `QRect`, which keeps `render/`
free of Qt for the price of three lines in the view.

### 4.4 `engine/` — new, Qt-free

```cpp
// engine/FrameClock.hpp

struct TickPlan {
    int ticks{0};
    std::chrono::steady_clock::duration discardedTime{};   // non-zero: could not keep up
};

// Fixed-timestep accumulator, isolated from Qt and from the simulation so it can be driven
// with synthetic elapsed times in tests.
//
// Precondition on the caller: wakes must arrive more frequently than the tick interval. A
// caller that wakes more slowly starves the accumulator on a fixed beat and produces
// zero-tick wakes at a predictable rate — see problem 9 and decision 6.7.
class FrameClock final {
public:
    FrameClock(int ticksPerSecond, int maximumTicksPerWake);

    [[nodiscard]] TickPlan consume(std::chrono::steady_clock::duration elapsed) noexcept;
    void reset() noexcept;
};
```

`TickPlan::discardedTime` is the fix for problem 2: the catch-up clamp becomes a returned
fact instead of a silent local adjustment.

```cpp
// engine/FrameReport.hpp

struct FrameReport {
    int ticksExecuted{0};
    std::chrono::steady_clock::duration elapsed{};         // wall-clock gap since the last wake
    std::chrono::steady_clock::duration discardedTime{};   // non-zero: could not keep up
    std::uint64_t frameIndex{0};

    [[nodiscard]] bool producedFrame() const noexcept { return ticksExecuted > 0; }
};

// engine/FireEngine.hpp

// The frame loop and the owner of the pure simulation-side state. Advancing is separate from
// rendering: the engine decides whether a new frame exists, and the view decides how to show
// it.
class FireEngine final {
public:
    FireEngine(std::size_t simulationWidth, std::size_t simulationHeight);

    [[nodiscard]] FrameReport advance(std::chrono::steady_clock::duration elapsed);
    void reset() noexcept;

    [[nodiscard]] HeatFrame heat() const noexcept;

    [[nodiscard]] const FireParameters& parameters() const noexcept;
    void setParameters(const FireParameters& parameters) noexcept;

    [[nodiscard]] const TimeSeriesMetric& simulateTime() const noexcept;
    [[nodiscard]] const TimeSeriesMetric& wakeInterval() const noexcept;
    [[nodiscard]] std::uint64_t totalTicks() const noexcept;
    [[nodiscard]] std::uint64_t totalFrames() const noexcept;

private:
    FireSimulation simulation;
    FrameClock clock;
    TimeSeriesMetric simulateMetric;
    TimeSeriesMetric wakeIntervalMetric;
    std::uint64_t tickTotal{0};
    std::uint64_t frameTotal{0};
};
```

`advance` is the whole loop:

```cpp
FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) {
    wakeIntervalMetric.record(elapsed);

    const TickPlan plan = clock.consume(elapsed);
    if (plan.ticks == 0) {
        // No tick means no new heat, so there is nothing to shade and nothing to present.
        // Shading here would convert unchanged heat into identical pixels and contaminate
        // the Shade metric with work that should not exist.
        return FrameReport{0, elapsed, plan.discardedTime, frameTotal};
    }

    {
        const ScopedTimer timer{simulateMetric};
        for (int tick = 0; tick < plan.ticks; ++tick) {
            simulation.tick();
        }
    }

    tickTotal += static_cast<std::uint64_t>(plan.ticks);
    return FrameReport{plan.ticks, elapsed, plan.discardedTime, ++frameTotal};
}
```

**Zero-tick policy.** A wake that produces no tick produces no frame, emits no signal, and
triggers no repaint. This preserves the `if (ticksExecuted > 0)` guard that today lives in
`FireController::advanceFrame`; dropping it would be a regression on roughly 4% of wakes
under the current timer settings.

`FireEngine` is testable headlessly: advance it with synthetic elapsed values and assert on
`heat()`, the reports, and the metrics.

`setParameters` taking a whole `FireParameters` value — rather than one setter per field —
resolves problem 5: two engine entry points regardless of knob count.

### 4.5 `app/` — Qt shell

| Class | Responsibility |
|-------|----------------|
| `FireController` | Qt adapter only. Owns `QTimer` and `FireEngine`; measures the wake interval and passes it to `advance`; emits `frameReady(FrameReport)` only when `producedFrame()`, plus `runningChanged(bool)`. |
| `FireView` (was `FireWidget`) | Owns a `FireRenderer`, a `PixelBuffer`, and its three metrics. `present(const HeatFrame&)` shades into its own buffer and calls `update()`; `paintEvent` records the paint-to-paint interval, then blits into `fitPreservingAspect(...)`. |
| `ControlPanel` | Emits `parametersChanged(FireParameters)`. Also offers `setParameters(const FireParameters&)` for external writes — see below. |
| `StatsPanel` | Displays a `FrameStatistics` value pushed into it. Holds no references and reads nothing itself. |
| `MainWindow` | Composition root: owns the simulation dimensions, constructs and wires everything, and runs the 250 ms stats refresh timer. |

**`FireView` owns everything it paints.** Its `QImage` wraps its own `PixelBuffer` using the
non-owning `QImage(const uchar*, int, int, Format)` constructor. Because the buffer and the
image are members of the same object, there is no cross-object borrow and no copy.

**Stats refresh is pulled, not pushed per frame.** `MainWindow` runs a 250 ms timer that
calls `statistics()` on each producer, assembles a `FrameStatistics` value, and hands it to
`StatsPanel`. Updating labels at 60 Hz would be both wasteful and unreadable; 4 Hz is legible
and keeps the per-frame path free of any reporting work.

```cpp
struct FrameStatistics {
    MetricStatistics simulate;
    MetricStatistics shade;
    MetricStatistics present;
    MetricStatistics wakeInterval;
    MetricStatistics presentInterval;
    double ticksPerFrame{0.0};
};
```

**Parameter binding is one-way, with an explicit escape hatch.** `ControlPanel` emits
`parametersChanged(FireParameters)`; nothing currently writes parameters from anywhere else,
so nothing pushes back. `ControlPanel::setParameters` exists for the case that changes:
it updates each slider under a `QSignalBlocker`, which is what prevents the write from
echoing back out as a fresh `parametersChanged` and looping. Until a second writer exists,
that method has no caller.

This is a partial fix for problem 6: the mechanism is specified and the loop-breaking rule is
stated, but the binding is not live. Claiming otherwise would be false — a naive live binding
would fight the user mid-drag.

`FireParameters` is passed by value through a signal on a direct, same-thread connection, so
no `qRegisterMetaType` is required. If that connection ever becomes queued, it will be.

---

## 5. Contracts and constraints

**Qt boundary.** `sim`, `render`, `engine` and `metrics` must never include a Qt header. Only
`classic_fire` links Qt. This extends the existing rule from `AGENTS.md` to the whole spine.

**Frame geometry.** `HeatFrame` carries the simulation geometry; `PixelBuffer` carries the
target geometry. No third party configures either. `SIMULATION_WIDTH` and
`SIMULATION_HEIGHT` exist at exactly one site — `MainWindow`, the composition root — and are
handed only to `FireEngine`. `FireView` never learns them; it sizes itself from the frames it
receives.

**Pixel format.** `Rgba32` is `0xFFRRGGBB` in native byte order, matching
`QImage::Format_RGB32`.

**No cross-object borrows.** Every object owns what it reads on the hot path:

| Object | Owns | Borrows |
|--------|------|---------|
| `FireEngine` | simulation, clock, its two metrics | — |
| `FireView` | renderer, `PixelBuffer`, `QImage` over it, its three metrics | — |
| `StatsPanel` | the last `FrameStatistics` value | — |

`HeatFrame` is the one borrow, and it is passed as an argument and never stored, so it cannot
outlive its source. There is deliberately **no** rule about construction or destruction
order: an earlier draft had `FireView` wrapping an engine-owned buffer, which made
correctness depend on Qt's child-destruction order — a rule nobody would remember and that
ASan would only catch at shutdown, once, on someone else's machine. Making the renderer
stateless removed the constraint rather than documenting it.

**Single-threaded rendering.** `QWidget::update()` posts a deferred paint request, so
`paintEvent` never runs nested inside the timer slot, and nothing on the render path spins a
nested event loop. Shading and painting are therefore strictly serialized by the event loop
and a single buffer is safe. This holds only while nothing calls synchronous `repaint()` from
the render path.

The same deferral means a shade whose `update()` is coalesced away is wasted work. That is
accepted — see decision 6.5.

---

## 6. Design decisions

Recorded so they are not silently re-litigated.

### 6.1 No `RenderPass` / `RenderContext` / `RenderPipeline` — rejected

A pass interface with a linear pipeline was considered and rejected. Its main argument was
that self-identifying stages let a pipeline register profiler channels automatically, so
instrumentation is not hand-written at each site. That argument does not survive contact with
the actual stage count: there are five channels, they are enumerated in section 3, and each
is recorded by the object that naturally owns the work. Two `ScopedTimer` scopes and three
interval records give the same property without a vtable, an owned
`vector<unique_ptr<RenderPass>>`, or a per-frame context struct.

Runtime pass ordering and polymorphic extension were the other arguments, and both are
speculative — there is exactly one pass.

**Cost of the decision:** adding a post-process effect later means editing
`FireRenderer::render` rather than adding one registration line; per-effect timing needs a
second `ScopedTimer` inside `render()`.

**Why it is safe:** nothing outside `render/` can observe the difference. `FireView` calls
`renderer.render(heat, buffer)` under either design, so the abstraction can be reintroduced
as a `render/`-internal refactor if a second effect ever justifies it.

### 6.2 No double buffering — rejected

See the single-threaded rendering constraint in section 5. The event loop already serializes
writes and reads, so a second buffer protects against nothing. It would be required if
shading ever moved off the UI thread; that change would be internal to `FireView`, since
`PixelBuffer` is already the unit that would be doubled.

### 6.3 No `RenderSettings` — deferred

A render-side settings value mirroring `FireParameters` was considered. It is deferred
because there is currently exactly one plausible render knob (palette choice), which
`FireRenderer::setPalette` covers, and a second candidate (smooth scaling) belongs to the
view rather than the renderer. Introduce the type when a second render knob actually exists.

### 6.4 No interpolation alpha — dropped

An `interpolationAlpha` on `TickPlan` was considered so the renderer could blend between
simulation states. With a stateless single-pass renderer it has no consumer, and an unused
field is dead weight. `FrameClock` can compute it in one line when something needs it.

### 6.5 The view owns the destination buffer; shading is pushed — accepted with a stated cost

An earlier draft had `FireEngine` own a `PixelBuffer` and shade into it, with `FireView`
wrapping that buffer. Two things were wrong with it. It made correctness depend on
destruction order (section 5), and it locked shading into the pure layer on the timer clock,
where the view had no say in when — or whether — it happened.

Making `FireRenderer::render(heat, target)` stateless and moving the destination into
`FireView` fixes both: the borrow disappears at zero copy cost, and the decision of when to
shade now belongs to the Qt layer, which is the only layer that knows about repaints.

**Shading is still pushed**, from `present()` rather than from `paintEvent`. A shade whose
`update()` is coalesced away is therefore wasted. This is accepted because the alternative —
shading lazily inside `paintEvent` — requires the view to hold the heat data until paint
time, which means either storing a `HeatFrame` that borrows simulation memory (reintroducing
exactly the lifetime coupling just removed) or copying the heat map every frame. Push
shading, with the zero-tick guard in `advance` in front of it, wastes work only on frames the
compositor was going to drop anyway.

If the `Present` and `PresentInterval` metrics ever show coalescing is frequent, the fix is
to move shading into `paintEvent` and accept the heat copy. The renderer is already stateless,
so that change touches only `FireView`.

### 6.6 No `FrameProfiler`, no channel registry — rejected

An earlier draft had a `FrameProfiler` with `registerChannel`, an opaque `MetricHandle`, and
a `ProfileSnapshot` holding a `vector<Channel>` that a generic panel iterated. It was
justified by the argument that adding a stage later would otherwise need coordinated edits in
three places.

That is the same speculative-extension argument decision 6.1 rejects, and it should not be
accepted here after being rejected there. There are five channels; they are enumerated in
this document; named members and labelled rows cost less and read better. The registry also
carried three defects the simpler design does not have:

- `snapshot()` allocated a `vector` and section 3 called it every frame, contradicting the
  "recording never allocates" discipline stated in the same section.
- `registerChannel` required names with static storage duration, an unenforceable
  precondition living in a comment.
- Ownership was unresolvable: a profiler shared between the engine and the view has to be
  owned above both, which pushes it into `main` and reintroduces exactly the injected-monitor
  shape this redesign exists to remove.

Each component now owns the metrics it produces and `MainWindow` pulls values, so nothing is
shared and nothing is injected.

`CounterMetric` was dropped for the same reason: it had no consumer. The two counts that do
have one — total ticks and total frames, for the ticks-per-frame ratio — are plain
`std::uint64_t` members on `FireEngine`.

### 6.7 Frame pacing is partially in scope — decided

**In scope, and fixed by this redesign:**

1. *Measurement.* `WakeInterval` and `PresentInterval` make pacing visible for the first
   time. Without them, a stats panel showing three stage durations would report 3 ms of work
   while the fire stutters, because the stutter lives in the interval, not the duration.
2. *The starvation beat* (problem 9). `WAKE_INTERVAL_MILLISECONDS` becomes **8**, strictly
   shorter than the 16.667 ms step, and `FrameClock` documents that as a precondition on its
   caller. This removes the systematic zero-tick beat: every step boundary is crossed by the
   wake that follows it rather than the one after that.
3. *The zero-tick regression guard*, so waking twice as often does not double the shading
   work. Wakes without a tick return early and cost one accumulator addition.

**The residual, which this does not fix.** Presentation is still quantized to the wake grid.
With 8 ms wakes and a 16.667 ms step, successive presents land 16 ms or 24 ms apart, so
roughly ±4 ms of jitter remains. Halving the wake interval again to 4 ms halves the jitter
and doubles the wakeups; that is a knob, not a solution.

**Out of scope: vsync-aligned presentation** — see section 8. `QWidget` exposes no
present-completion callback, so genuinely smooth pacing requires driving presentation from
the compositor's clock via `QRhiWidget` or `QOpenGLWidget`. That is a different application
architecture and is not proposed here.

An implementation that lands all six migration steps and changes nothing else will therefore
pace **better** than today — no starvation beat, no redundant shading — but not perfectly.
Anyone expecting frame-perfect smoothness should read this section as a refusal.

---

## 7. Migration plan

Each step builds, ships, and leaves the application working.

1. **Test target, `fire_metrics`, and a `FireSimulation` regression test.** Establishes
   CTest. Covers `TimeSeriesMetric` statistics, and — using the fixed seed that already
   exists for this purpose — hashes the heat map after N ticks and asserts on it. That golden
   test is what makes steps 2 through 6 safe, and it is the test problem 8 actually named.
2. **`HeatFrame`, and `FireSimulation::heat()` returning it.** One new header, one signature
   change, call sites updated. Resolves problem 4 on the producing side.
3. **`fire_render`: `PixelBuffer`, `FirePalette`, `FireRenderer`, `Viewport`, plus tests.**
   New target, no callers yet. Palette endpoints, buffer geometry, and letterbox arithmetic
   all become testable.
4. **`FireWidget` becomes `FireView`.** Adopts the renderer, the buffer, and its metrics;
   loses palette construction and the conversion loop. Resolves problem 3.
5. **`FrameClock` plus tests, and the 8 ms wake interval.** `FireController` loses the
   accumulator. Tests assert that 16 ms wakes produce the starvation beat and 8 ms wakes do
   not. Resolves problems 2 and 9.
6. **`FireEngine`, `FrameReport`, and `setParameters`.** `FireController` becomes a Qt
   adapter; `ControlPanel` sends a whole value. Resolves problems 1 and 5, and problem 6 to
   the extent described in section 4.5.
7. **`StatsPanel` and the 250 ms refresh.** The feature that motivated the redesign; by this
   point it is additive.

Step 1 comes first specifically because it is the only pure addition and it delivers the
regression net the rest depends on. Steps 2 through 4 were a single step in an earlier draft;
they are split because a new target, four new types, a signature change and a widget rewrite
cannot honestly claim to "leave the application working" as one commit.

Problem 7 is addressed incrementally as targets are split in steps 1, 3 and 6, to the extent
described in section 2.

---

## 8. Out of scope

Named explicitly so they are not built speculatively:

- **Vsync-aligned presentation.** The residual pacing jitter described in decision 6.7 would
  need presentation driven by the compositor, which `QWidget` does not expose. It would mean
  moving to `QRhiWidget` or `QOpenGLWidget` — a different application architecture.
- **Threaded shading.** Would require reinstating double buffering inside `FireView`.
- **Pull shading** (shading inside `paintEvent`). See decision 6.5 for the condition that
  would justify revisiting it.
- **Frame interpolation.** See decision 6.4.
- **Post-process effects** (glow, bloom, scanlines). See decision 6.1.
- **A general frame graph** with declared resources and dependency sorting.
- **A GPU path.** `PixelBuffer` and `FirePalette` map onto a texture upload and a LUT sampler
  if that ever becomes interesting, but nothing here should be shaped around it today.

---

## 9. Open questions

- Whether `Viewport` earns its own translation unit or should stay a private helper on
  `FireView`. It is extracted here for testability; the arithmetic is small.
- Whether 8 ms is the right wake interval, or whether 4 ms is worth the extra wakeups. This
  is answerable with data once `PresentInterval` exists — which is an argument for landing
  step 5 and step 7 close together.
- Whether `StatsPanel` should show the raw discarded-time total, or only surface it as a
  warning state when it is non-zero.
ENDOFDOC
wc -l docs/render_pipeline_redesign.md && grep -c '```' docs/render_pipeline_redesign.md
````

<details>
<summary>Result</summary>

```
     713 docs/render_pipeline_redesign.md
20
```

</details>

Rewrote [docs/render_pipeline_redesign.md](docs/render_pipeline_redesign.md) — 713 lines. Most of the review changed the design, not just the prose.

## Dispositions

| Concern | Disposition |
|---|---|
| Framing oversells | Added a **Scope** section up top + **problem 9** (frame pacing) with the arithmetic, and §6.7 stating exactly which part gets fixed |
| Instrumentation can't see pacing | `FrameReport.elapsed` added; two interval channels added — `WakeInterval`, `PresentInterval`. §3 now says plainly that durations answer "where does time go" and only intervals answer "is it smooth" |
| `advance()` shades unconditionally | Explicit early return on `plan.ticks == 0`, with the policy stated and the reason commented in the sketch. `FrameReport::producedFrame()` gates the signal |
| Push-shading locked in | Took your `render(heat, PixelBuffer&)` — renderer stateless, view owns the buffer |
| Lifetime rule | **Gone**, not documented. §5 now has a no-cross-object-borrows table and says why the ordering rule was removed rather than written down |
| `metrics/` over-built | Registry cut entirely. Three types left: `MetricStatistics`, `TimeSeriesMetric`, `ScopedTimer` |
| `snapshot()` allocates per frame | No vector anywhere; `statistics()` returns by value. Stats are pulled at 4 Hz |
| `CounterMetric` has no consumer | Dropped. Tick/frame totals are two `uint64_t` members on `FireEngine` |
| `registerChannel` precondition | Gone with the registry |
| Two-way binding claim | Claim dropped; problem 6 downgraded to partially addressed |
| `FireSimulation` untested | Now in step 1 — fixed-seed golden hash, alongside CTest setup |
| Step 2 too big | Split into steps 2/3/4; migration is 7 steps |
| §2 dependency arrow | Fixed — `engine → metrics`, `app → metrics`; `sim` and `render` link neither |
| `MainWindow` contradiction | Resolved — it *is* the composition root and owns the dimensions; `FireView` never learns them |
| `PaletteStop` undeclared | Declared |
| 60 Hz StatsPanel | 250 ms refresh timer in `MainWindow`, pull-and-push, panel holds no references |

## Three places I went further or differently

**Buffer ownership — took inversion, not the copy.** You offered either. Inversion is strictly better here and costs nothing. Also, the copy was cheaper in your estimate than in reality: the target is 800×600×4 = **1.92 MB**, not a few hundred KB, so it's ~150–200 µs per frame rather than ~15 µs. Still affordable, but inversion is free, so the choice isn't close.

**Metrics went past "three named members."** A profiler with named members still has to be *shared* between `FireEngine` and `FireView`, which means owning it above both — i.e. in `main()` — which is precisely the injected-monitor shape this redesign exists to kill. So each component now owns the metrics it produces and `MainWindow` pulls values. No profiler type at all. That's recorded as §6.6, which also states outright that accepting a registry after rejecting `RenderPass` on the same argument was inconsistent.

**Pacing got a partial fix, not just a scope declaration.** `WAKE_INTERVAL_MILLISECONDS` goes 16 → **8**, and `FrameClock` documents "wake faster than the step" as a caller precondition, with a test in step 5 asserting the beat appears at 16 ms and not at 8 ms. §6.7 then states the residual honestly: presentation stays quantized to the wake grid, so gaps alternate 16/24 ms — roughly ±4 ms jitter — and closing that needs compositor-driven presentation, which is §8 out-of-scope with the reason (`QWidget` has no present callback; it means `QRhiWidget`/`QOpenGLWidget`). The section ends by saying anyone expecting frame-perfect smoothness should read it as a refusal.

## Still open for you

I kept the filename and addressed the framing with Scope + problem 9 — the option you offered second. If you'd still rather have `docs/architecture_redesign.md`, that's a one-line `git mv` and a title change; say the word.

The file remains untracked — nothing committed, nothing in `src/` touched.

## 👤 User  
*2026-08-25 18:22:52.808 UTC*

One more review round. Do no change documment immediately, but rather confirm or deny concerns

## 👤 User  
*2026-08-25 18:22:57.334 UTC*

[Request interrupted by user]

## 👤 User  
*2026-08-25 18:23:07.771 UTC*

One more review round. Do no change documment immediately, but rather confirm or deny concerns

Verdict

Substantially better. Every point from the last round is addressed, and several are addressed better than I suggested — making FireRenderer::render stateless kills the lifetime constraint and the buffer-ownership awkwardness in one move, which is a cleaner answer than the copy I proposed. §6.5 and §6.6 are model design records: they name the earlier draft, say what was wrong with it, and state the residual cost.

One real error remains, and it sits in the newest material.

The pacing analysis is inverted

Problem 9 defines the defect as wakes that produce zero ticks — "about 4% of them produce zero ticks and no new frame" (:60). §6.7 then claims the 8 ms wake "removes the systematic zero-tick beat" (:632).

It does the opposite. At 8 ms you take 125 wakes per second and still produce 60 ticks, so 65 wakes per second produce zero ticks — 52%, up from 4%. §6.7's own item 3 already knows this ("so waking twice as often does not double the shading work"), which puts the two items in direct contradiction three lines apart.

What actually improves is the thing problem 9 never names: the gap between presented frames.

	wakes/s	zero-tick wakes/s	frame gaps	worst gap
16 ms	62.5	2.5 (4%)	57.5 × 16 ms, 2.5 × 32 ms	+15.3 ms
8 ms	125	65 (52%)	55 × 16 ms, 5 × 24 ms	+7.3 ms

So the change is real and worth making — worst-case pacing error halves — and §6.7's residual paragraph (:640) describes the 16/24 ms outcome correctly. The defect is that the problem statement and the fix statement are both written in terms of zero-tick wakes, which is the one quantity that gets dramatically worse. Under the new design a zero-tick wake isn't a symptom at all; it's the expected majority case.

This propagates into an unwritable test. Step 5 says "Tests assert that 16 ms wakes produce the starvation beat and 8 ms wakes do not" (:669). If "starvation beat" means zero-tick wakes, that assertion fails. The test has to assert on the distribution of gaps between ticks — that no gap exceeds 24 ms at 8 ms wakes, versus 32 ms at 16 ms wakes.

Restate problem 9 as "presented frames are spaced 16 or 32 ms apart against a 16.667 ms ideal," rewrite 6.7 item 2 as halves rather than removes, and the test writes itself.

Also uncounted: 8 ms doubles timer wakeups to 125/s permanently, on a laptop. §6.7 charges that cost against the 4 ms option but not against the 8 ms one it adopts.

Two wiring gaps

Nothing says how MainWindow reaches the engine's metrics. FireController "Owns QTimer and FireEngine" (:446), and the 250 ms refresh needs simulateTime(), wakeInterval(), totalTicks(), and totalFrames() from it — plus heat() for the present() call in §3. Either the controller forwards all six, which rebuilds the forwarding facade problem 5 exists to remove, or it exposes const FireEngine& engine() const. Pick one and write it down; right now the doc's own §4.5 responsibility line for FireController doesn't permit either.

ticksPerFrame is a lifetime ratio (:468), computed from two monotonic counters. Frames only exist when ticks > 0, so after a minute of running it sits pinned near 1.000 and a three-second stall moves it by nothing. §3 sells it as the number that "distinguishes 'the timer is late' from 'the simulation is slow'" — it can't, at that resolution. Make it a windowed ratio or drop it and read the same story off WakeInterval versus PresentInterval.

Smaller
PresentInterval will be polluted. It's recorded unconditionally in paintEvent, which also fires on resize and expose. While paused, update() stops but expose events don't, so a resize injects a multi-second "interval" into the metric. Guard it on running, or record only paints triggered by present().
statistics() is const, noexcept, by-value, allocation-free, and computes a p95 (:218). That combination is achievable — copy the ring into a fixed stack array and nth_element — but it's non-obvious enough to deserve the one line saying so, along with the actual MAXIMUM_SAMPLE_COUNT.
"six migration steps" appears twice (:17, :647); §7 now has seven.
Problem 3 is claimed resolved by step 4, and the palette and conversion loop do become independently testable — but FireView ends up owning a renderer, a buffer, a QImage, three metrics, the shade call, the paint, and the interval recording. The extraction is the win; the responsibility count isn't down. Worth one honest sentence, in the style the rest of the doc uses.
The §2 diagram doesn't draw app → render, though the bullets and the target table both have it. Cosmetic, as is the over-long border on the engine box.
The uncached bilinear upscale on every repaint still isn't named anywhere. Measuring it via the Present channel is a defensible answer — measure before optimizing — but §8 would be the place to say "known cost, deferred pending data" so it isn't rediscovered as a surprise.
Where that leaves it

Fix the problem 9 / §6.7 framing and the step 5 test assertion, decide the MainWindow→engine access path, and this is ready to implement. Everything else on the list is a line or two of text. The design itself I'd build as written.

## 🤖 Assistant  
*2026-08-25 18:25:15.892 UTC*

Reviewed all of it. **Confirming the central finding — and it's worse than diagnosed, with a better fix available than either of us proposed.** Nothing changed in the file yet.

## Dispositions

| # | Concern | Verdict |
|---|---|---|
| 1 | Pacing analysis inverted | **Confirmed**, plus a second error inside the paragraph you credited as correct |
| 2 | Step 5 test unwritable | **Confirmed** |
| 3 | 8 ms doubles wakeups, uncharged | **Confirmed** |
| 4 | No `MainWindow` → engine access path | **Confirmed** |
| 5 | `ticksPerFrame` is a lifetime ratio | **Confirmed**, and it cascades further than you said |
| 6 | `PresentInterval` polluted by expose/resize | **Confirmed** |
| 7 | `statistics()` combination non-obvious; `MAXIMUM_SAMPLE_COUNT` never given | **Confirmed** |
| 8 | "six steps" ×2 vs seven | **Confirmed** |
| 9 | Problem 3: extraction is the win, responsibility count isn't down | **Confirmed** |
| 10 | Diagram missing `app → render`; long border | **Confirmed** |
| 11 | Uncached upscale unnamed | **Confirmed** |

## 1–3. Pacing — confirmed, and I had two errors, not one

Your arithmetic checks out exactly. Working it as `n_k = ceil(step·k / wake)`:

| Wake | Wakes/s | Zero-tick wakes/s | Tick gap pattern | Worst gap error |
|---|---|---|---|---|
| 16 ms | 62.5 | 2.5 (**4%**) | 57.5 × 16 ms, 2.5 × 32 ms | +15.33 ms |
| 8 ms | 125 | 65 (**52%**) | 55 × 16 ms, 5 × 24 ms | +7.33 ms |

At 16 ms the beat is `ceil(25k/24)` — a doubled gap every 24 ticks. At 8 ms it's `ceil(25k/12)` — every 12 ticks. Both rows reproduce your table.

So §6.7 item 2 is flatly wrong: zero-tick wakes go 4% → 52%. And item 3 knows it three lines later. Confirmed.

**The error you credited as correct is also wrong.** You wrote that §6.7's residual paragraph "describes the 16/24 ms outcome correctly." The gaps are right, but I characterised them as "roughly ±4 ms of jitter." Against a 16.667 ms ideal the deviations are −0.67 ms and **+7.33 ms** — not symmetric, and the number that matters is more than 4. Your table has it right; my sentence doesn't.

### There's a third option that beats 8 ms on every axis

Both of us treated the wake interval as the only free variable. The step is equally free — and the invariant that actually matters is **ticks/s ≥ wakes/s**, which can be satisfied from either side.

Keep the 16 ms wake and set the step to **1/64 s = 15.625 ms**:

| | Wakes/s | Zero-tick wakes | Present gaps | Worst error |
|---|---|---|---|---|
| 16 ms wake, 64 Hz step | 62.5 | **0** | uniformly 16 ms | −0.67 ms |

`n_k = ceil(0.9765625k)` increments by 0 or 1 and covers every wake, so every wake fires at least one tick, ~1.5 wakes/s fire two, and the present cadence is flat 16 ms. No starvation, no doubled gaps, **no extra wakeups** — which also answers your uncharged-cost point, since it doesn't incur it.

Cost: the fire animates 6.7% faster. It's a visual effect with no physical time constant; `cooling` compensates if anyone cares.

This makes `FrameClock`'s documented precondition the general one — *the caller must not wake more often than the clock ticks* — and the test writes itself either way: assert max inter-tick gap, not zero-tick counts. Your restatement of problem 9 ("presented frames are spaced 16 or 32 ms apart against a 16.667 ms ideal") is the right framing and works for this option too.

## 4. Wiring gap — confirmed; recommending the single const accessor

`FireController` owns the engine, and §4.5 grants it no way to expose it, while §3 needs `heat()` and the refresh needs four more. Real gap.

I'd add `[[nodiscard]] const FireEngine& engine() const noexcept` and say why it isn't problem 5 returning: that problem was *mutating* pass-throughs multiplying one-per-parameter. One read-only accessor doesn't multiply — it's the same shape as `parameters()` today.

Not `MainWindow` owning the engine: as a `MainWindow` member it would be destroyed before `~QObject` deletes the controller, which is exactly the destruction-order hazard §5 just eliminated.

## 5. `ticksPerFrame` — confirmed, and it cascades

Pinned near 1.000 by construction, since frames only exist when ticks ≥ 1. After a minute: ~3600 ticks / ~3550 frames = 1.014; a three-second stall moves it to ~1.019. §3's claim that it separates "timer is late" from "simulation is slow" is unsupportable at that resolution — and redundant, because `WakeInterval` and `PresentInterval` already separate exactly those two.

**Further than you took it:** `totalTicks` and `totalFrames` exist *only* to feed that ratio. Drop it and they have no consumer — which is the same test §6.6 used to drop `CounterMetric`. They should go with it, or §6.6 becomes inconsistent for the second time.

## 6–7. Confirmed

`PresentInterval`: recording unconditionally in `paintEvent` means a resize while paused injects a multi-second sample. Prefer your second fix — a flag set in `present()` and cleared in `paintEvent`, recording only flagged paints. That also handles coalescing correctly: two presents collapsing into one paint yields one interval, which is what the viewer actually saw.

`statistics()`: the const/noexcept/by-value/allocation-free/p95 combination is achievable — copy the ring into a fixed stack array, `nth_element` — but it needs the sentence. `MAXIMUM_SAMPLE_COUNT` is referenced and never defined; I'd set 256.

**One you didn't raise:** with a fixed sample count, channels cover different wall-clock spans — `Present*` fills at ~60/s (≈4 s) while `WakeInterval` fills at the wake rate. Worth a line so nobody compares windows that aren't the same length.

## 8–11. Confirmed, all text-level

Including your read on the upscale: measuring it through `Present` before optimising is the right call; §8 just needs to say "known cost, deferred pending data" so it isn't rediscovered as a surprise.

## Two more I found while checking

- **§3 contradicts §4.4 on who records `WakeInterval`.** §3's flow and channel table attribute it to `FireController::onWake`; §4.4's `advance()` records it into a `FireEngine` member. The engine version is correct — the controller only measures and passes `elapsed` — so §3 is the one to fix.
- **`FrameReport.frameIndex` semantics are unstated.** The early-return path returns the un-incremented `frameTotal`, so zero-tick wakes repeat the previous frame's index. That's defensible, but it means `frameIndex` counts *frames*, not wakes, and nothing says so.

## Proposed change list

Pending your go-ahead, and I'd want a decision on the first item before touching §6.7:

1. **Pacing:** restate problem 9 in terms of present gaps; rewrite §6.7 item 2 as *halves* not *removes*; fix the "±4 ms" to "−0.7/+7.3 ms". **Open question for you: 8 ms wake, or 64 Hz step at the existing 16 ms wake?** I'd take the latter — strictly better on every measured axis and it costs nothing.
2. Rewrite step 5's assertion as max inter-tick gap.
3. Add `FireController::engine()` with the rationale.
4. Drop `ticksPerFrame`, `totalTicks`, `totalFrames`; note in §6.6 that the same test retired them.
5. Guard `PresentInterval` on a present-triggered flag.
6. Define `MAXIMUM_SAMPLE_COUNT = 256`; add the no-allocation p95 note and the differing-window-span caveat.
7. Fix "six steps" → seven; fix the §3/§4.4 `WakeInterval` contradiction; state `frameIndex` semantics.
8. Add the honest sentence on `FireView`'s responsibility count to §4.5 or problem 3's resolution note.
9. Diagram: add `app → render`, trim the engine border.
10. §8: name the uncached upscale as known-cost-deferred.

Items 2–10 are mechanical. Say the word on item 1 and I'll apply the set.
