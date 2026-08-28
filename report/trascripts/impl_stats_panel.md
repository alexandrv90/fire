# Sketch StatsPanel implementation

> I want you to work step 9 of migration plan recorded in [render\_pipeline\_redesign.md](docs/render_pipeline_redesign.md) which is implementing StatsPanel.
>
> Before taking any actions I want you to present a proposed sketch.
> I think it should be implemented as a rectangle with opaque background and text on in the top left corner of the window. I should not be in the rendering path.

> 🤝 Following project rules.

Proposed sketch:

```text
┌──────────────────────────────────────────────┐
│ ┌──────────────────────┐                     │
│ │ FPS: 60.0            │                     │
│ │ Frame: 16.7 ms       │     Render view     │
│ │ Simulation: 4.2 ms   │                     │
│ │ Entities: 12,480     │                     │
│ └──────────────────────┘                     │
│                                              │
└──────────────────────────────────────────────┘
```

Implementation concept:

- A `StatsPanel` Qt widget layered over the render view in the window’s top-left corner.
- Opaque rectangular background, compact padding, and left-aligned monospace text.
- Owned and positioned by the UI/window layer—not by the renderer, render graph, or frame composition path.
- Updated from already-collected metrics through a UI-facing method or signal.
- Metrics cross the simulation boundary only as pure C++ data; no Qt types enter the simulation library.
- Hidden by default if that matches step 9, with a lightweight toggle if the documented plan requires one.
- Panel updates can be throttled independently so text layout does not run every rendered frame.

No repository inspection or modification has been performed yet.

> I dont think it should be a child of Render view. Explore source code and make more informed sketch about architecture rather than looks

<details><summary>6 previous messages</summary>

> You’re right—the ownership question needs to come from the existing window/widget structure. I’ll inspect the required project docs and trace window creation, render-surface ownership, and metrics flow, then return an architecture-focused sketch only; I won’t modify code.
>
> <details><summary>Explored 3 files</summary>
>
> - Read `./README.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
>
> </details>
>
> The design document already points toward `MainWindow` as the composition owner, but I’m checking the implemented steps rather than treating the plan as current truth. The key question is whether the panel fits as a sibling in the window layout, a dock/tool widget, or an overlay owned directly by the top-level window.
>
> <details><summary>Explored 12 files, a search, a list</summary>
>
> - Listed files
> - Searched for `class (MainWindow|FireView|FrameMetricsCollector|FireController|ControlPanel)|FrameMetricsSnapshot|Q(MainWindow|Widget|Layout|DockWidget|StackedLayout)|setCentralWidget|addWidget|paintMeasured|frameMeasured|wakeMeasured` in `CMakeLists.txt`
> - Read `./CMakeLists.txt`
> - Read `./src/app/MainWindow.hpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./src/app/FrameMetricsCollector.hpp`
> - Read `./src/app/FrameMetricsCollector.cpp`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/app/ControlPanel.hpp`
> - Read `./src/app/ControlPanel.cpp`
> - Read `./src/main.cpp`
>
> </details>
>
> The implemented structure confirms that `FireView` is intentionally presentation-only, while `MainWindow` already owns all cross-component wiring. I’m narrowing the sketch around a dedicated render-area host so the stats UI is a sibling overlay and its polling dependency points only at the collector.
>
> <details><summary>Explored 6 files</summary>
>
> - Read `./src/app/MainWindow.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/app/FrameMetricsCollector.hpp`
> - Read `./src/app/FrameMetricsCollector.cpp`
> - Read `./docs/render_pipeline_redesign.md`
>
> </details>
</details>

Agreed: `StatsPanel` should not be owned by `FireView`. The existing architecture supports a sibling overlay composed by `MainWindow`.

```text
MainWindow                         composition root
├── FrameMetricsCollector          session-owned metric storage
├── FireController
└── centralWidget
    └── QVBoxLayout
        ├── renderArea             new plain QWidget
        │   └── overlay layout
        │       ├── FireView       full-area presentation surface
        │       └── overlayLayer   full-area, mouse-transparent
        │           └── StatsPanel opaque, aligned top-left
        └── ControlPanel
```

Data flow remains separate:

```text
FireController ──measurements──┐
                               v
FireView ───────paint data──> FrameMetricsCollector
                               ^
                               │ snapshot() every 250 ms
                           StatsPanel
```

Recommended responsibilities:

- `MainWindow` creates and wires everything, consistent with its current composition-root role in [MainWindow.cpp](./src/app/MainWindow.cpp:13).
- `FireView` remains unaware of `StatsPanel`; its API and paint implementation stay unchanged. It continues to do only frame presentation and paint measurement, as seen in [FireView.hpp](./src/app/FireView.hpp:9).
- `StatsPanel` lives entirely under `app/`. It takes a non-owning reference to `FrameMetricsCollector`, owns its 250 ms refresh timer, calls `snapshot()`, and updates only its own labels.
- `FrameMetricsCollector` remains the sole aggregation owner. Its existing snapshot already contains the five channels and latest `FrameReport` needed by step 9: [FrameMetricsCollector.hpp](./src/app/FrameMetricsCollector.hpp:13).
- No `frameReady` connection goes to `StatsPanel`; its refresh cadence is independent of frame production, matching the documented 4 Hz decision in [render_pipeline_redesign.md](./docs/render_pipeline_redesign.md:477).
- No change is needed in `engine/`, `render/`, `FireController`, or `FireView`.

The `renderArea` is only a UI composition container. A stacked layout gives `FireView` and the overlay layer the same geometry. The overlay layer ignores mouse events, while `StatsPanel` occupies only its size hint in the top-left corner and paints an opaque background.

This keeps panel painting outside `FireView::paintEvent`, so it is also outside the measured fire-presentation duration. It still necessarily participates in Qt’s normal widget painting, but it does not become a simulation, shading, frame-production, or fire-presentation stage.

I would initially display the five existing metric channels and the latest report fields. I would not add counters yet because the migration document still records those as an unresolved scope expansion.

> You can start implementation, initial state - visible, metrics collection - enabled

<details><summary>10 previous messages</summary>

> I’ll implement the sibling-overlay architecture from the sketch. The panel will start visible, its own 4 Hz timer will poll the collector, and `MainWindow` will continue enabling metrics collection at startup. I’ll add focused UI tests, build and run the full suite, then format only the touched C++ files as the final step.
>
> <details><summary>Ran a command</summary>
>
> - Ran `git status --short && sed -n '1,220p' src/metrics/MetricStatistics.hpp && sed -n '1,220p' src/engine/FrameReport.hpp && sed -n '1,300p' tests/app/ControlPanelTests.cpp && sed -n '1,300p' tests/app/FrameMetricsCollectorTests.cpp && sed -n '1,220p' CMakePresets.json && command -v clang-format || true`
>
> </details>
>
> The implementation will keep the existing metric producers untouched. `StatsPanel` will only depend on the collector’s read-only snapshot API; its opaque widget is placed in a mouse-transparent overlay layer above `FireView`, with both owned by a neutral render-area container.
>
> <details><summary>Created 3 files, edited 3 files, ran a command</summary>
>
> - Wrote `./CMakeLists.txt` (+24 -0)
> - Wrote `./docs/render_pipeline_redesign.md` (+3 -2)
> - Wrote `./src/app/MainWindow.cpp` (+24 -2)
> - Wrote `./src/app/StatsPanel.cpp` (+115 -0)
> - Wrote `./src/app/StatsPanel.hpp` (+21 -0)
> - Wrote `./tests/app/StatsPanelTests.cpp` (+111 -0)
> - Ran `git diff --check && git diff --stat && sed -n '1,260p' src/app/StatsPanel.cpp && sed -n '1,180p' src/app/MainWindow.cpp && sed -n '120,245p' CMakeLists.txt`
>
> </details>
>
> The panel now has a fixed snapshot display for all five existing channels plus the latest frame report. It starts visible and refreshes immediately, then every 250 ms; the existing startup call still enables collection before the controller starts producing frames. Next I’m compiling the new target and its test to catch Qt 5/6 compatibility issues before running everything.
>
> <details><summary>Ran 3 commands</summary>
>
> - Ran `cmake --preset cfg-debug && cmake --build --preset build-debug --target fire_app_stats_panel_tests classic_fire`
> - Ran `ctest --test-dir build/debug --output-on-failure -R fire_app_stats_panel_tests`
> - Ran `cmake --build --preset build-debug && ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> The first build exposed no compiler issues. During the pre-final review I found one Qt paint-contract detail worth tightening: an opaque widget should explicitly paint every background pixel instead of combining the opaque attribute with automatic background filling. I’m correcting that and adding a direct assertion for the 250 ms refresh timer before the final full verification.
>
> <details><summary>Edited 4 files, ran 2 commands</summary>
>
> - Wrote `./src/app/MainWindow.cpp` (+1 -0)
> - Wrote `./src/app/StatsPanel.cpp` (+11 -1)
> - Wrote `./src/app/StatsPanel.hpp` (+4 -0)
> - Wrote `./tests/app/StatsPanelTests.cpp` (+7 -1)
> - Ran `cmake --build --preset build-debug --target fire_app_stats_panel_tests classic_fire && ctest --test-dir build/debug --output-on-failure -R fire_app_stats_panel_tests`
> - Ran `cmake --build --preset build-debug && ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> All seven debug tests now pass, including the new offscreen panel test. The final code change is complete; I’m applying the repository formatter only to the four touched C++ source/header files, then I’ll inspect the final diff and repository status without making further edits.
>
> <details><summary>Ran a command</summary>
>
> - Ran `clang-format -i src/app/MainWindow.cpp src/app/StatsPanel.cpp src/app/StatsPanel.hpp tests/app/StatsPanelTests.cpp && git diff --check && git status --short && git diff -- CMakeLists.txt docs/render_pipeline_redesign.md src/app/MainWindow.cpp && git diff --no-index /dev/null src/app/StatsPanel.hpp || true && git diff --no-index /dev/null src/app/StatsPanel.cpp || true && git diff --no-index /dev/null tests/app/StatsPanelTests.cpp || true`
>
> </details>
</details>

Implemented migration step 9.

- Added [StatsPanel](./src/app/StatsPanel.cpp:36) with an opaque background, five metric rows, latest frame data, and independent 4 Hz refresh.
- Composed it as a sibling overlay—not a child of `FireView`—in [MainWindow.cpp](./src/app/MainWindow.cpp:25).
- Panel starts visible; metrics collection remains enabled before the controller starts.
- Added offscreen UI tests in [StatsPanelTests.cpp](./tests/app/StatsPanelTests.cpp:28).
- Updated the migration document to mark steps 1–9 complete.

Verification: debug build succeeded and all 7 tests passed. Modified C++ files were formatted with the repository’s `clang-format`.

> Drop n column, add to the first row instead: Window {rolling window size} s | {samples num} samples

<details><summary>6 previous messages</summary>

> I’ll trace how the rolling window is actually defined and which channel provides the representative sample count, then update the header and focused test without changing collection behavior.
>
> <details><summary>Explored 4 files, a search</summary>
>
> - Read `./src/metrics/TimeSeriesMetric.hpp`
> - Read `./src/metrics/TimeSeriesMetric.cpp`
> - Read `./src/metrics/IntervalMetric.hpp`
> - Read `./src/metrics/IntervalMetric.cpp`
> - Searched for `MAXIMUM_SAMPLE_COUNT|sampleCount|rolling window|Window` in `src`
>
> </details>
>
> The underlying storage is sample-based (512 records), not a fixed-duration window. I’ll make the header report the actual time span represented by the wake-interval window—`average interval × retained samples`—along with that window’s current sample count. This gives a truthful seconds value as the window fills.
>
> <details><summary>Edited 3 files, ran a command</summary>
>
> - Wrote `./src/app/StatsPanel.cpp` (+18 -12)
> - Wrote `./src/app/StatsPanel.hpp` (+1 -1)
> - Wrote `./tests/app/StatsPanelTests.cpp` (+10 -7)
> - Ran `cmake --build --preset build-debug && ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> The revised header and table pass the complete test suite. I’m now applying the required final formatting to the three changed C++ files and checking the resulting diff.
>
> <details><summary>Ran a command</summary>
>
> - Ran `clang-format -i src/app/StatsPanel.cpp src/app/StatsPanel.hpp tests/app/StatsPanelTests.cpp && git diff --check && git status --short && git diff --stat`
>
> </details>
</details>

Updated `StatsPanel`:

- Removed the `n` column.
- First row now displays `Window 8.53 s | 512 samples`.
- Window duration is derived from the retained wake intervals.
- Empty state displays `Window 0.00 s | 0 samples`.

All 7 tests pass, and touched C++ files were formatted.

> Add new button next to Pause to toggle metrics - both disable collection and hide stats panel. Stats panel is not expected to be show if metrics collection is disabled, you can simplify its code accordingly

<details><summary>4 previous messages</summary>

> I’ll make the collector’s enabled state the single source of truth: a checkable `Metrics` button sends the desired state to `FrameMetricsCollector`; the collector then synchronizes the button, producers, and panel. When disabled, `StatsPanel` will hide and stop its refresh timer, so it performs no snapshot/statistics work.
>
> <details><summary>Edited 7 files, ran a command</summary>
>
> - Wrote `./docs/render_pipeline_redesign.md` (+3 -2)
> - Wrote `./src/app/ControlPanel.cpp` (+11 -0)
> - Wrote `./src/app/ControlPanel.hpp` (+3 -0)
> - Wrote `./src/app/MainWindow.cpp` (+8 -0)
> - Wrote `./src/app/StatsPanel.cpp` (+11 -5)
> - Wrote `./tests/app/ControlPanelTests.cpp` (+34 -0)
> - Wrote `./tests/app/StatsPanelTests.cpp` (+15 -12)
> - Ran `cmake --build --preset build-debug && ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> The toggle path is working in the full suite: the button’s checked state round-trips through the collector, and disabling collection now hides the panel and stops its timer. I’m applying final formatting to the touched C++ files and validating the diff.
>
> <details><summary>Ran a command</summary>
>
> - Ran `clang-format -i src/app/ControlPanel.cpp src/app/ControlPanel.hpp src/app/MainWindow.cpp src/app/StatsPanel.cpp tests/app/ControlPanelTests.cpp tests/app/StatsPanelTests.cpp && git diff --check && git status --short && git diff --stat`
>
> </details>
</details>

Added a checkable `Metrics` button next to Pause.

- Toggling off disables collection, hides `StatsPanel`, and stops its refresh timer.
- Toggling on clears/restarts collection, shows the panel, and resumes 4 Hz updates.
- Collector state remains the single source of truth for the button and panel.
- Updated control and panel tests plus architecture documentation.

All 7 tests pass; touched C++ files were formatted.