# Add configurable fire palettes

> I want to create configurable palette for fire.
>
> Suggested design:
>
> - 3 precompiled presets - be creative
> - Each preset has a name
> - New Dropbox between reset and heat slider in control panel.
>
> Present a sketch how we can wire list of presets to dropdown

<details><summary>4 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll inspect the current control panel and simulation/rendering boundaries, then sketch a concrete wiring that fits the existing architecture without changing code yet.
>
> <details><summary>Explored a file, a search</summary>
>
> - Read `./README.md`
> - Searched for `Reset|reset|heat|Heat|slider|QComboBox|Control` in `.`
>
> </details>
>
> The current project already has a clean palette seam: `FireRenderer` owns a pure-C++ `FirePalette`, while `ControlPanel` emits app-layer signals through `MainWindow` to `FireController`. I’m tracing those exact APIs now so the sketch can use the smallest coherent extension.
>
> <details><summary>Explored 20 files</summary>
>
> - Read `./src/engine/FirePalette.hpp`
> - Read `./src/engine/FirePalette.cpp`
> - Read `./src/engine/FireRenderer.hpp`
> - Read `./src/engine/FireRenderer.cpp`
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/engine/FireEngine.cpp`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/app/ControlPanel.hpp`
> - Read `./src/app/ControlPanel.cpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./tests/engine/RenderTests.cpp`
> - Read `./tests/app/ControlPanelTests.cpp`
> - Read `./src/engine/CMakeLists.txt`
> - Read `./src/engine/FirePalette.hpp`
> - Read `./src/engine/FireRenderer.hpp`
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/ControlPanel.cpp`
> - Read `./src/app/MainWindow.cpp`
>
> </details>
</details>

The clean design is to keep palettes out of `FireParameters`: heat/cooling affect simulation, while palettes belong to rendering.

Proposed presets:

- **Furnace Gold** — current black → crimson → orange → gold → white.
- **Ghostlight** — black → deep teal → emerald → mint → white.
- **Arcane Bloom** — black → violet → magenta → icy blue → white.

UI sketch:

```text
[Metrics] [Pause] [Reset]   Palette [Furnace Gold ▾]   Heat ──●── 255   Cooling ─●── 2
```

Wiring:

```text
FirePalettePreset catalog
    │  {stable ID, display name, color stops}
    ▼
QComboBox
    │ palettePresetSelected(ID)
    ▼
FireController::setPalettePreset(ID)
    ▼
FireEngine::setPalettePreset(ID)
    ▼
FireRenderer::setPalette(...) + render current heat frame
    ▼
frameReady()
```

A pure-C++ catalog could look like:

```cpp
enum class FirePalettePresetId : std::uint8_t {
    FurnaceGold,
    Ghostlight,
    ArcaneBloom,
};

struct FirePalettePreset {
    FirePalettePresetId id;
    std::string_view name;
    std::span<const PaletteStop> stops;
};

[[nodiscard]] std::span<const FirePalettePreset> firePalettePresets() noexcept;
[[nodiscard]] const FirePalettePreset& firePalettePreset(FirePalettePresetId id);
```

The dropdown should retain stable IDs separately from its visible names:

```cpp
for (const FirePalettePreset& preset : presets) {
    palettePresetIds.push_back(preset.id);
    paletteComboBox->addItem(QString::fromUtf8(preset.name));
}

connect(paletteComboBox,
        &QComboBox::currentIndexChanged,
        this,
        [this](const int index) {
            if (index >= 0) {
                emit palettePresetSelected(
                    palettePresetIds[static_cast<std::size_t>(index)]);
            }
        });
```

Then add these connections beside the existing ones in [MainWindow.cpp](./src/app/MainWindow.cpp:54):

```cpp
connect(controlPanel,
        &ControlPanel::palettePresetSelected,
        fireController,
        &FireController::setPalettePreset);

connect(fireController,
        &FireController::palettePresetChanged,
        controlPanel,
        &ControlPanel::setPalettePreset);
```

`FireEngine::setPalettePreset()` should immediately re-shade the existing heat map. That makes palette changes visible while paused and avoids resetting the simulation. Reset should preserve the selected palette.

The existing extension point is already present in [FireRenderer.hpp](./src/engine/FireRenderer.hpp:11), and the dropdown belongs directly after the Reset button in [ControlPanel.cpp](./src/app/ControlPanel.cpp:56). No code was changed for this sketch.

> Drop label "Palette", only dropdown stays. Keep classic pallete. You can implement it now

<details><summary>10 previous messages</summary>

> I’ll implement the three built-in presets with **Classic** retained as the default, add only the dropdown between Reset and Heat, and make selection re-render immediately without resetting the simulation.
>
> <details><summary>Ran a command</summary>
>
> - Ran `git status --short; sed -n '1,180p' CMakeLists.txt; sed -n '1,130p' src/app/CMakeLists.txt; sed -n '1,180p' cmake/FireTargets.cmake 2>/dev/null || true; rg -n "Q_DECLARE_METATYPE|Q_ENUM|currentIndexChanged|QComboBox" src tests`
>
> </details>
>
> There are existing user changes in the exact engine/controller files this feature touches. I’m reviewing both staged and unstaged diffs first so the palette work composes with them rather than overwriting them.
>
> <details><summary>Explored a file, a search, ran 2 commands</summary>
>
> - Ran `git diff -- README.md src/app/FireController.cpp src/app/FireController.hpp src/engine/FireEngine.cpp src/engine/FireEngine.hpp; git diff --cached -- README.md src/app/FireController.cpp src/app/FireController.hpp src/engine/FireEngine.cpp src/engine/FireEngine.hpp`
>
> - Ran `sed -n '1,190p' tests/engine/FrameClockTests.cpp; sed -n '1,120p' tests/app/FireControllerTests.cpp; sed -n '1,150p' cmake/ProjectSettings.cmake; command -v clang-format || true; ls -d build-* 2>/dev/null || true; cmake --list-presets=build`
>
> <details><summary>Explored a file, a search</summary>
>
> - Read `./CMakePresets.json`
> - Searched for `*build*` in `.`
>
> </details>
>
> </details>
>
> The existing staged `noexcept` edits are compatible and will be preserved. The implementation will use a stable pure-C++ preset ID plus a single catalog containing each name and stop list; the Qt dropdown will store the ID as item data, so display names and ordering are not used as behavior.
>
> <details><summary>Edited 14 files, ran a command</summary>
>
> - Wrote `./README.md` (+1 -1)
> - Wrote `./src/app/ControlPanel.cpp` (+36 -1)
> - Wrote `./src/app/ControlPanel.hpp` (+8 -1)
> - Wrote `./src/app/FireController.cpp` (+10 -0)
> - Wrote `./src/app/FireController.hpp` (+3 -0)
> - Wrote `./src/app/MainWindow.cpp` (+4 -1)
> - Wrote `./src/engine/FireEngine.cpp` (+12 -1)
> - Wrote `./src/engine/FireEngine.hpp` (+4 -0)
> - Wrote `./src/engine/FirePalette.cpp` (+41 -1)
> - Wrote `./src/engine/FirePalette.hpp` (+17 -0)
> - Ran `git diff --check; git diff -- src/engine/FirePalette.hpp src/engine/FirePalette.cpp src/engine/FireEngine.hpp src/engine/FireEngine.cpp src/app/FireController.hpp src/app/FireController.cpp src/app/ControlPanel.hpp src/app/ControlPanel.cpp src/app/MainWindow.cpp README.md`
> - Wrote `./tests/app/ControlPanelTests.cpp` (+38 -0)
> - Wrote `./tests/app/FireControllerTests.cpp` (+32 -0)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+23 -0)
> - Wrote `./tests/engine/RenderTests.cpp` (+25 -0)
>
> </details>
>
> The feature and test coverage are now in place: catalog validation, engine re-shading/persistence, controller notifications, and dropdown behavior—including an assertion that no “Palette” label exists. I’m compiling before the required final formatting pass so any interface or Qt meta-object issues surface first.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `cmake --build --preset build-debug`
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> The debug build succeeds and all 9 test executables pass. I’m now applying the repository’s required final `clang-format` pass to only the C++ files touched by this task, then I’ll do a post-format verification and inspect the final diff for accidental overlap.
>
> <details><summary>Explored 4 files, ran 2 commands</summary>
>
> - Ran `clang-format -i src/engine/FirePalette.hpp src/engine/FirePalette.cpp src/engine/FireEngine.hpp src/engine/FireEngine.cpp src/app/FireController.hpp src/app/FireController.cpp src/app/ControlPanel.hpp src/app/ControlPanel.cpp src/app/MainWindow.cpp tests/engine/RenderTests.cpp tests/engine/FrameClockTests.cpp tests/app/FireControllerTests.cpp tests/app/ControlPanelTests.cpp; git diff --check; cmake --build --preset build-debug; ctest --test-dir build/debug --output-on-failure`
>
> - Ran `git status --short; git diff --stat HEAD; git diff --check HEAD; git diff HEAD -- src/engine/FirePalette.hpp src/engine/FirePalette.cpp src/engine/FireEngine.hpp src/engine/FireEngine.cpp src/app/FireController.hpp src/app/FireController.cpp src/app/ControlPanel.hpp src/app/ControlPanel.cpp src/app/MainWindow.cpp tests/engine/RenderTests.cpp tests/engine/FrameClockTests.cpp tests/app/FireControllerTests.cpp tests/app/ControlPanelTests.cpp README.md`
>
> <details><summary>Explored 4 files</summary>
>
> - Read `./src/engine/FirePalette.hpp`
> - Read `./src/app/ControlPanel.cpp`
> - Read `./src/engine/FireEngine.cpp`
> - Read `./src/app/MainWindow.cpp`
>
> </details>
>
> </details>
</details>

Implemented configurable fire palettes.

- Added **Classic**, **Ghostlight**, and **Arcane Bloom** presets in [FirePalette.hpp](./src/engine/FirePalette.hpp:17).
- Added the unlabeled dropdown between Reset and Heat in [ControlPanel.cpp](./src/app/ControlPanel.cpp:63).
- Palette changes immediately re-shade the current frame, including while paused.
- Reset preserves the selected palette.
- Added catalog, engine, controller, and UI tests.
- Preserved the existing staged `noexcept` changes.

Verification: debug build succeeded and all 9 tests passed. Touched C++ files were formatted with the repository’s `clang-format` configuration.

> Add rule to [AGENTS.md](AGENTS.md) to avoid exceptions in hot path, they are only allowed in initialization code.

<details><summary>2 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll add the hot-path exception restriction to the authoritative coding rules in `AGENTS.md`, keeping the wording explicit about initialization-only use.
>
> <details><summary>Edited a file, explored a file, ran 3 commands</summary>
>
> - Ran `sed -n '1,260p' AGENTS.md; git status --short -- AGENTS.md`
>
> <details><summary>Explored a file</summary>
>
> - Read `./README.md`
>
> </details>
>
> - Wrote `./AGENTS.md` (+1 -1)
>
> - Ran `git diff --check -- AGENTS.md; git diff -- AGENTS.md; nl -ba AGENTS.md | sed -n '25,38p'`
>
> - Wrote `./AGENTS.md` (+0 -0)
>
> - Ran `git diff --check -- AGENTS.md; git diff -- AGENTS.md`
>
> </details>
</details>

Added the exception policy to [AGENTS.md](./AGENTS.md:34):

> Exceptions are permitted only during initialization. Hot-path and steady-state runtime code must use non-throwing APIs and explicit error results.

No tests were run because this was documentation-only.

> Rework FirePalette::fromStops. Make it non-throwing. If something is wrong with provided preset fallback to classic.

<details><summary>6 previous messages</summary>

> I’ll make palette construction fully non-throwing, use the compiled Classic stops as the safe fallback without recursive factory calls, and normalize unknown preset IDs to Classic so runtime state cannot retain an invalid ID.
>
> <details><summary>Edited 6 files, ran a command</summary>
>
> - Ran `git status --short; sed -n '1,150p' src/engine/FirePalette.hpp; sed -n '1,180p' src/engine/FirePalette.cpp; sed -n '30,68p' src/engine/FireEngine.cpp; sed -n '36,68p' src/app/FireController.cpp; sed -n '45,125p' tests/engine/RenderTests.cpp; rg -n "fromStops|fromPreset|firePalettePreset\(" src tests`
> - Wrote `./src/app/FireController.cpp` (+4 -3)
> - Wrote `./src/app/FireController.hpp` (+1 -1)
> - Wrote `./src/engine/FireEngine.cpp` (+5 -4)
> - Wrote `./src/engine/FireEngine.hpp` (+1 -1)
> - Wrote `./src/engine/FirePalette.cpp` (+31 -15)
> - Wrote `./src/engine/FirePalette.hpp` (+4 -4)
>
> </details>
>
> The implementation now validates stops before interpolation and substitutes the Classic stop table on any structural error. Unknown preset IDs are also canonicalized to Classic before engine/controller state changes; next I’m replacing the old exception assertions with fallback and `noexcept` coverage.
>
> <details><summary>Edited 2 files, ran 2 commands</summary>
>
> - Wrote `./tests/engine/FrameClockTests.cpp` (+4 -0)
> - Wrote `./tests/engine/RenderTests.cpp` (+36 -13)
> - Ran `git diff --check; rg -n "throw|invalid_argument|checkThrows" src/engine/FirePalette.* tests/engine/RenderTests.cpp; cmake --build --preset build-debug`
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> The non-throwing implementation compiles and all 10 current tests pass, including the user’s newly present simulation tests. I’m applying the required final formatting pass to only the palette-related files, then re-running verification.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `clang-format -i src/engine/FirePalette.hpp src/engine/FirePalette.cpp src/engine/FireEngine.hpp src/engine/FireEngine.cpp src/app/FireController.hpp src/app/FireController.cpp tests/engine/RenderTests.cpp tests/engine/FrameClockTests.cpp; git diff --check; cmake --build --preset build-debug; ctest --test-dir build/debug --output-on-failure`
> - Ran `git diff --check; git diff -- src/engine/FirePalette.hpp src/engine/FirePalette.cpp src/engine/FireEngine.hpp src/engine/FireEngine.cpp src/app/FireController.hpp src/app/FireController.cpp tests/engine/RenderTests.cpp tests/engine/FrameClockTests.cpp; nl -ba src/engine/FirePalette.cpp | sed -n '42,125p'; nl -ba src/engine/FirePalette.hpp | sed -n '25,42p'; nl -ba src/engine/FireEngine.cpp | sed -n '38,56p'`
>
> </details>
</details>

Reworked palette creation to be fully non-throwing.

- [`FirePalette::fromStops()`](./src/engine/FirePalette.cpp:87) now validates stops and falls back to Classic for empty, incomplete, duplicate, or descending definitions.
- Unknown preset IDs also resolve to Classic.
- Palette factories and runtime selection APIs are now `noexcept`.
- Built-in presets are compile-time validated with `static_assert`.
- Updated tests to verify complete palette equality with Classic on fallback.

Debug build succeeded; all 10 tests passed.
