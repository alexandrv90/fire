# Implement metrics collector

> Your task is to implement step 7 of migration plan recorded in [render\_pipeline\_redesign.md](docs/render_pipeline_redesign.md)&#x20;

<details><summary>12 previous messages</summary>

> > 🤝 Following project rules.
>
> I’ll first read the repository overview and the full render-pipeline plan, then trace the code and tests affected by step 7 before implementing it.
>
> <details><summary>Explored 3 files</summary>
>
> - Read `./README.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
>
> </details>
>
> The plan defines step 7 narrowly: add the pure-C++ `FireEngine`, move simulation/render/profiling ownership and frame production into it, and reduce `FireController` to timer/run-state adaptation while preserving the existing pacing. I’m checking the completed steps and current ownership/signals now.
>
> <details><summary>Explored 25 files, a search, ran 2 commands</summary>
>
> - Ran `git status --short && rg --files -g '!build*' -g '!out*' | sort && rg -n "class FireController|FrameClock|FrameReport|FrameProfiler|frameReady|advanceFrame|FireEngine|parametersChanged|present\(" CMakeLists.txt src tests 2>/dev/null`
>
> <details><summary>Explored 25 files, a search</summary>
>
> - Read `./CMakeLists.txt`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/app/MainWindow.hpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/sim/FireSimulation.hpp`
> - Read `./src/sim/FireSimulation.cpp`
> - Read `./src/sim/FireParameters.hpp`
> - Read `./src/sim/FireParameters.cpp`
> - Read `./src/render/FireRenderer.hpp`
> - Read `./src/render/FireRenderer.cpp`
> - Read `./src/engine/FrameClock.hpp`
> - Read `./src/engine/FrameClock.cpp`
> - Read `./src/metrics/FrameProfiler.hpp`
> - Read `./src/metrics/FrameProfiler.cpp`
> - Searched for `reset\(\)|frameIndex|discardedTime|profiler\(\)|setParameters|parametersChanged|FireEngine` in `docs`
> - Read `./src/app/ControlPanel.hpp`
> - Read `./src/app/ControlPanel.cpp`
> - Read `./tests/engine/FrameClockTests.cpp`
> - Read `./tests/render/RenderTests.cpp`
> - Read `./src/metrics/TimeSeriesMetric.hpp`
> - Read `./src/metrics/TimeSeriesMetric.cpp`
> - Read `./src/metrics/ScopedTimer.hpp`
> - Read `./src/metrics/ScopedTimer.cpp`
>
> </details>
>
> - Ran `git log --oneline --decorate -12 && git log -p -4 -- src/app/FireController.cpp src/engine/FrameClock.cpp docs/render_pipeline_redesign.md | sed -n '1,420p'`
>
> </details>
>
> The existing steps 1–6 are clean and match the plan. I’ll add `fire_engine` as its own Qt-free target, implement `FrameReport`/`FireEngine` with headless tests, then rewire the controller and view so wake, simulation, shade, and presentation metrics land on the intended profiler channels. The current slider API will remain operational through temporary wiring in `MainWindow`; its value-binding rewrite belongs to step 8.
>
> <details><summary>Created 3 files, edited 7 files, ran 2 commands</summary>
>
> - Ran `git status --short && sed -n '135,205p' CMakeLists.txt`
> - Wrote `./src/engine/FireEngine.cpp` (+39 -0)
> - Wrote `./src/engine/FireEngine.hpp` (+38 -0)
> - Wrote `./src/engine/FrameReport.hpp` (+11 -0)
> - Wrote `./src/app/FireController.cpp` (+19 -22)
> - Wrote `./src/app/FireController.hpp` (+11 -15)
> - Wrote `./src/app/FireView.cpp` (+3 -1)
> - Wrote `./src/app/FireView.hpp` (+3 -1)
> - Wrote `./src/app/MainWindow.cpp` (+15 -5)
> - Wrote `./CMakeLists.txt` (+22 -7)
> - Wrote `./src/app/MainWindow.cpp` (+1 -0)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+75 -1)
> - Ran `git diff --check && git diff --stat && git diff -- CMakeLists.txt src/app src/engine tests/engine/FrameClockTests.cpp`
>
> </details>
>
> The ownership split is now in place. Before polishing, I’m compiling the new target and Qt wiring; this should catch both ordinary C++ issues and Qt MOC compatibility for the new value-carrying signals.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `cmake --preset cfg-release && cmake --build --preset build-release`
> - Ran `ctest --test-dir build/release --output-on-failure`
>
> </details>
>
> The release build and all four test targets pass. I’m doing the architecture review now—checking that no Qt dependency leaked below `app/`, that target links match the documented layering, and that reset/zero-tick behavior and profiler ownership remain internally consistent—before the required final formatting pass.
>
> <details><summary>Edited a file, ran 2 commands</summary>
>
> - Ran `git status --short
> rg -n "#include <Q|#include \"Q|Qt::|QObject|QWidget" src/engine src/render src/sim src/metrics || true
> cmake --build build/release --target help | rg '^\.\.\. (fire_engine|classic_fire|fire_.*tests)($|:)' || true
> rg -n "FireSimulation|FireRenderer|FrameClock|setSourceHeat|setCooling|frameReady\(\)" src/app CMakeLists.txt`
> - Wrote `./tests/engine/FrameClockTests.cpp` (+5 -2)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+5 -2)
> - Ran `cmake --preset cfg-sanitize && cmake --build --preset build-sanitize && ctest --test-dir build/sanitize --output-on-failure`
>
> </details>
>
> Both release and ASan/UBSan builds now pass all tests. I’m doing one short offscreen application launch to exercise the Qt event-loop path and new signal wiring, then I’ll format only the touched C++ files as the repository’s required final step.
>
> <details><summary>Explored 3 files, ran 3 commands</summary>
>
> - Ran `QT_QPA_PLATFORM=offscreen ./build/release/classic_fire.app/Contents/MacOS/classic_fire`
>
> - Ran `clang-format -i src/app/FireController.cpp src/app/FireController.hpp src/app/FireView.cpp src/app/FireView.hpp src/app/MainWindow.cpp src/engine/FireEngine.cpp src/engine/FireEngine.hpp src/engine/FrameReport.hpp tests/engine/FrameClockTests.cpp`
>
> - Ran `git diff --check && git status --short && git diff --stat && git diff -- CMakeLists.txt src/app src/engine tests/engine/FrameClockTests.cpp`
>
> <details><summary>Explored 3 files</summary>
>
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/engine/FireEngine.cpp`
> - Read `./src/engine/FrameReport.hpp`
>
> </details>
>
> </details>
</details>

Implemented migration plan step 7.

- Added Qt-free [`FireEngine`](./src/engine/FireEngine.hpp) and [`FrameReport`](./src/engine/FrameReport.hpp).
- Moved simulation, rendering, frame-clock, frame indexing, and profiling into the engine.
- Reduced [`FireController`](./src/app/FireController.hpp) to a Qt timer adapter emitting report-bearing frame signals.
- Wired wake and presentation interval metrics.
- Added the documented `fire_engine` CMake target and expanded engine tests.
- Preserved current controls pending step 8.

Validation:

- Release build: passed
- Sanitizer build: passed
- All 4 test targets: passed
- Offscreen Qt application smoke test: passed
- Modified C++ files formatted with repository `clang-format` configuration.

> This implementatio follows the spec well. However, I want you to run analysis whether current approach to integrating FrameProfiler and Metrics is reasonable.
>
> - FrameProfiler is burried inside FireEngine but exposed to other component as well. IntervalMetric is passed to a FireView constructor, which is a weird thing to do. What if we want to enable/disable profiling. It would require code modifications. It feels like a singleton approach would work better here&#x20;
> - FrameProfiler name is a bit confusing. It doesn't really profile anything, just collects stats
>
> Do not modify any code, only run analysis and suggest improvement options

<details><summary>2 previous messages</summary>

> > 🤝 Following project rules.
>
> I’ll evaluate the current ownership and lifetime model against the stated architecture, then compare singleton/global access with explicit alternatives—including how each would support runtime enable/disable—without changing files.
>
> <details><summary>Explored 16 files</summary>
>
> - Read `./src/metrics/FrameProfiler.hpp`
> - Read `./src/metrics/FrameProfiler.cpp`
> - Read `./src/metrics/IntervalMetric.hpp`
> - Read `./src/metrics/IntervalMetric.cpp`
> - Read `./src/metrics/TimeSeriesMetric.hpp`
> - Read `./src/metrics/TimeSeriesMetric.cpp`
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/engine/FireEngine.cpp`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
>
> </details>
</details>

The current approach is reasonable as an incremental implementation of the documented design, but I agree it should not be the final architecture if runtime control and cleaner instrumentation boundaries matter. A singleton would simplify access, but it would trade the visible awkwardness for hidden global coupling.

## What is good today

Embedding the metrics in [`FireEngine`](./src/engine/FireEngine.hpp:24) provides:

- Clear lifetime: metrics live exactly as long as the engine.
- Per-engine isolation, which is useful in tests and if multiple engines ever exist.
- No global initialization or synchronization concerns.
- Explicit instrumentation sites.

Passing a reference to [`FireView`](./src/app/FireView.hpp:11) is also memory-safe and explicit. Constructor injection itself is not the problem.

## What is awkward

There are three genuine design smells:

1. **Ownership does not match scope.**
   The metrics describe work performed by `FireEngine`, `FireController`, and `FireView`, but are owned by only one of them. They are application/session metrics, not strictly engine state.

2. **Storage details leak through several layers.**
   `FireEngine` exposes a mutable `FrameProfiler`, `FireController` forwards it, `MainWindow` selects one public member, and `FireView` receives that concrete metric. This expression captures the problem:

   ```cpp
   fireController->profiler().presentInterval
   ```

   Callers can also accidentally clear or write the wrong channel.

3. **`FireView` depends on how measurements are stored.**
   The view’s real event is “a paint completed.” It should not need to know that this event is being accumulated in an `IntervalMetric`.

The current design also has no defined enable/disable semantics. Adding an `enabled` flag to the existing aggregate would stop storage, but `ScopedTimer` would still call `Clock::now()` unless timing itself were gated.

## Why I would not use a singleton

A singleton makes access convenient:

```cpp
FrameMetrics::instance().markPresent();
```

But it introduces:

- Hidden dependencies in every producer.
- Cross-test contamination and mandatory global resets.
- Accidental aggregation if two windows or engines exist.
- Future synchronization requirements if rendering becomes threaded.
- Process-wide lifetime where the natural scope is one application or rendering session.

Runtime enable/disable does not require global access. It only requires one centrally owned policy object. A singleton solves lookup, not instrumentation policy.

## Improvement options

| Option | Advantages | Drawbacks |
|---|---|---|
| Keep current ownership; rename and add `enabled` | Smallest change | Ownership mismatch and `FireView` injection remain |
| Inject a metrics sink/null object | Explicit, testable, can eliminate disabled overhead | Adds an instrumentation interface to producers |
| Application-owned event collector | Cleanest component boundaries; view exposes only semantic events | Requires additional signals and possibly report fields |
| Singleton/service locator | Minimal constructor wiring | Hidden global state and poor isolation |

### Recommended: application-owned event collector

I would separate raw observations from rolling statistics:

- Rename the passive aggregate to `FrameMetrics` or `FrameMetricsCollector`.
- Own it at application/session scope, likely alongside the controller and view.
- Make its channels private.
- Give it semantic methods such as `observeWake`, `observeFrame`, and `observePresent`.
- Have `StatsPanel` read an immutable snapshot.
- Let `FireView` emit a `presented()` signal after painting instead of accepting an `IntervalMetric&`.
- Let `FireController` expose a wake event.
- Put simulate/shade durations in `FrameReport`, or send them as a separate engine observation.

The resulting flow would be:

```text
FireEngine ------ FrameReport with stage timings -----+
FireController -- wake event -------------------------+--> FrameMetricsCollector --> StatsPanel
FireView -------- presented event --------------------+
```

This removes metrics knowledge from `FireView`, prevents arbitrary channel mutation, and places ownership where all producers naturally meet.

`FrameMetricsCollector::setEnabled(false)` can then ignore observations. If “disabled” must also eliminate calls to `steady_clock::now()`, introduce an optional/gated instrumentation façade for the engine’s scoped timers. That is a separate requirement from merely stopping collection and display.

## Naming

I agree that `FrameProfiler` is misleading. It does not initiate profiling; it is a passive collection of rolling measurements.

Good alternatives:

- `FrameMetrics` — best if it remains primarily storage.
- `FrameMetricsCollector` — best if it receives observations through methods.
- `FrameTelemetry` — appropriate if it becomes the application-wide measurement subsystem.
- Reserve `FrameProfiler` for an active object that starts timers, creates measurement scopes, and controls whether profiling is enabled.

My recommendation is therefore: **do not introduce a singleton; move ownership to application/session scope, rename the type to `FrameMetricsCollector`, encapsulate its channels, and replace the metric reference in `FireView` with a semantic presentation event.**

No files were modified.

> Before goind any further run analysis and give me a summary - what metrics we're collecting, what each of them should indicate when evaluating real-time rendering performance.

The system currently collects four rolling timing metrics, plus contextual data in `FrameReport`. Together they answer two different questions: “How expensive is the work?” and “How regularly does the viewer receive frames?”

Each metric reports average, 95th percentile, maximum, and sample count over the latest 512 samples—roughly 8.5 seconds at 60 FPS.

| Metric | Measures | What it indicates |
|---|---|---|
| `simulate` | Total time executing all simulation ticks for one produced frame | CPU cost of advancing the fire model |
| `shade` | Time converting the heat frame into RGBA pixels | CPU cost of producing the displayable pixel buffer |
| `wakeInterval` | Time between consecutive `QTimer` callbacks | Stability of the timer and UI event loop |
| `presentInterval` | Time between consecutive `paintEvent` executions | Cadence at which Qt actually paints frames |

## Stage durations

### `simulate`

This measures the entire simulation loop for a wake:

```text
simulate duration = time for ticksExecuted simulation ticks
```

It should reveal whether simulation is consuming a significant portion of the approximately 16.67 ms frame budget at 60 Hz.

Interpretation:

- Low average and low p95: simulation has comfortable headroom.
- High p95: recurring expensive simulation frames.
- Large isolated maximum: occasional CPU scheduling interruption or system hitch.
- Duration growing when `ticksExecuted` is 2 or 3: expected catch-up work.

Important limitation: this is duration per produced frame, not per simulation tick. A three-tick catch-up frame naturally costs approximately three times as much as a one-tick frame. The rolling statistics currently lose that correlation, so a high simulation p95 does not by itself prove that individual ticks became slower.

### `shade`

This measures `FireRenderer::render`: mapping every heat value through the palette and writing the `PixelBuffer`.

It should reveal whether CPU pixel conversion is a bottleneck.

It does not include:

- Copying the `PixelBuffer` into `QImage`
- Qt painting
- Bilinear scaling
- Compositor work
- Actual display scanout

Consequently, a low shade duration does not mean the complete presentation path is inexpensive.

The stage averages can give a rough work estimate:

```text
engine CPU work ≈ average simulate + average shade
```

Percentiles should not be added directly because their slow samples may come from different frames.

## Interval metrics

### `wakeInterval`

This measures the cadence of the 16 ms `QTimer`.

Expected behavior is approximately 16 ms, with ordinary operating-system and event-loop variation.

It should indicate:

- Whether the UI thread is being scheduled regularly.
- Whether another UI operation is blocking timer delivery.
- Long stalls that cause the engine to execute multiple catch-up ticks.
- Timer jitter independent of how expensive simulation and shading are.

A stable wake interval with expensive stages means the application is approaching its CPU budget but is still receiving regular callbacks. An unstable wake interval while stages remain cheap points toward event-loop contention, OS scheduling, or unrelated main-thread work.

### `presentInterval`

This is the closest current approximation to perceived pacing. At 60 FPS, a regular result would be near 16.67 ms.

It should indicate:

- Whether paints occur at a stable cadence.
- Whether Qt coalesces multiple `update()` requests.
- Whether frames are repeated or effectively skipped.
- Whether painting cadence differs from timer cadence.

Examples:

- Stable wakes but irregular presents: likely paint scheduling, update coalescing, resizing, compositor interaction, or expensive painting.
- Wake and present intervals spike together: likely a blocked UI thread or system-wide scheduling stall.
- Present intervals around 33 ms or 50 ms: likely one or two nominal display opportunities were missed.

The name is slightly optimistic: it measures `paintEvent` cadence, not actual monitor presentation or photon latency. `paintInterval` would be technically more precise.

## `FrameReport` context

`FrameReport` is not currently a rolling metric, but its fields are essential for interpreting the timings:

- `ticksExecuted`
  - `0`: expected occasionally because the 16 ms timer is shorter than the 16.67 ms simulation step.
  - `1`: normal frame.
  - `2–3`: catch-up work after a delayed wake.

- `discardedTime`
  - Non-zero means the engine exceeded the three-tick catch-up limit.
  - This is the strongest direct indication that the application could not keep up and intentionally dropped simulation time.

- `elapsed`
  - Wall time accounted for by the wake.
  - Helps explain catch-up ticks and discarded time.

- `frameIndex`
  - Counts produced frames, not timer wakes.
  - Its divergence from wake count reflects zero-tick wakes.

## Evaluation priority

For real-time rendering, I would read the measurements in this order:

1. **`presentInterval`** — is displayed motion regular?
2. **`wakeInterval`** — is the event loop supplying work regularly?
3. **`discardedTime` and `ticksExecuted`** — is the engine falling behind or catching up?
4. **`simulate` and `shade`** — which CPU stage explains the pressure?

The most important distinction is that stage durations measure throughput, while intervals measure pacing. A frame can take only 2 ms to compute and still look uneven if paints arrive at 16, 32, 8, and 24 ms intervals. Conversely, a relatively expensive pipeline can still look smooth if it consistently completes within budget.

> Before the refactoring on main branch there was additional stage duration - how long QPainter::ctor->drawImage->QPainter::\~dtor takes. Is there any reason to discard this metric? What can it show?

<details><summary>4 previous messages</summary>

> I’ll inspect the pre-refactor paint instrumentation on `main` so I can compare its exact measurement boundary with the current `presentInterval` rather than reasoning from the label alone.
>
> <details><summary>Ran a command</summary>
>
> - Ran `git ls-tree -r --name-only main src | sort
> git grep -n "QPainter\|paintEvent\|ScopedTimer\|Metric\|profil" main -- src docs || true
> git show main:src/app/FireWidget.cpp 2>/dev/null || true
> git show main:src/app/FireWidget.hpp 2>/dev/null || true
> git show main:src/app/FireController.cpp 2>/dev/null || true
> git show main:src/app/FireController.hpp 2>/dev/null || true`
>
> </details>
>
> The local `main` ref currently contains the same `QPainter` scope but no committed timer around it, so I can’t verify the former channel’s exact name or aggregation from that branch. The boundary you described is nevertheless precise; I’m checking the redesign decisions to determine whether dropping it was deliberate or simply an omission.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `git branch -a
> git log --all --oneline --decorate -- src/app/FireWidget.cpp src/app/FireView.cpp src/metrics
> git log --all -G "QPainter.*ScopedTimer|ScopedTimer.*QPainter|paint.*Metric|Metric.*paint|present.*duration" -p -- src || true
> rg -n "paint|QPainter|upscale|scaled|duration|stage" docs/render_pipeline_redesign.md`
> - Ran `git show perf-monitor-cdx:src/app/FireWidget.cpp | sed -n '45,90p'
> git show perf-monitor-cdx:src/app/PerformanceMonitor.hpp | sed -n '1,130p'
> git show perf-monitor-cdx:src/app/PerformancePanel.cpp | sed -n '105,155p'`
>
> </details>
</details>

There is no strong reason to discard it. `paintEvent` duration is not redundant with `presentInterval`; the two answer different questions:

- `paintEvent` duration: how much UI-thread CPU time painting consumes.
- `presentInterval`: how regularly paint events occur.

The redesign retained the known CPU upscale in [`FireView::paintEvent`](./src/app/FireView.cpp:52), so removing its duration leaves a meaningful blind spot.

## What the metric measures

With the timer surrounding `QPainter` construction through destruction, it includes:

- Starting the widget paint engine.
- Filling the background.
- Configuring smooth scaling.
- Calculating the fitted destination rectangle.
- Scaling and drawing the `QImage`.
- Work performed when `QPainter` ends or flushes during destruction.

Because `SmoothPixmapTransform` is enabled, the image upscale may be a significant part of this duration, especially with a large window or high device-pixel ratio.

It can reveal:

- CPU cost of bilinear image scaling.
- Cost growth as the window becomes larger.
- Expensive resize or expose-event painting.
- Repeated painting while paused, when the source pixels have not changed.
- Platform/backend-specific `QPainter` performance.
- Occasional paint-path hitches through p95 and maximum values.
- How long painting occupies the UI thread and therefore delays timer delivery or input processing.

## How it complements the other metrics

Some useful combinations are:

| Observation | Likely interpretation |
|---|---|
| Low `shade`, high paint duration | Qt scaling/blitting is the rendering bottleneck |
| High paint duration and unstable `wakeInterval` | Painting is blocking the UI event loop and delaying timer callbacks |
| Low paint duration but unstable `presentInterval` | Paint scheduling, update coalescing, compositor cadence, or external scheduling is responsible |
| Paint duration rises sharply during resize | Destination scaling cost depends strongly on output size |
| High paint maximum but healthy p95 | Isolated expose, resize, allocation, or system hitch |
| Stable paint duration but 33 ms present intervals | Individual paints are cheap, but they are not being scheduled every display interval |

This is particularly useful because `presentInterval` alone cannot distinguish “painting took too long” from “Qt did not schedule a paint promptly.”

## What it does not measure

The boundary is still not true end-to-end presentation latency. It generally does not include:

- Time waiting before `paintEvent` begins.
- Compositor scheduling after Qt returns.
- Vsync waiting.
- Display scanout.
- Time until pixels become visible.

Some Qt backends may defer work beyond `QPainter` destruction, so the value should be described as **paint callback CPU duration**, not GPU time or screen-presentation duration.

## One important caveat

The historical performance panel summed simulation, conversion, and paint averages into a single “CPU budget.” That is potentially misleading in the redesigned pipeline:

- Simulation and shading occur on timer wakes.
- Multiple shaded frames can be coalesced into one paint.
- Paints can happen without a new frame, such as during resizing or while paused.
- A simulation sample may contain one, two, or three ticks.
- The independent rolling samples are not paired by frame.

Therefore, paint duration should be displayed as its own metric. It should only be included in an end-to-end frame total if individual simulation, shade, copy, and paint observations are correlated to the same frame.

## Recommendation

Retain both concepts as separate channels:

- `paintDuration`: duration from immediately before `QPainter` construction until immediately after its destruction.
- `paintInterval` or `presentInterval`: time between paint events.

I would favor the names `paintDuration` and `paintInterval`; they accurately describe what Qt is measuring without implying actual monitor presentation.

Architecturally, the view does need to establish the timing boundary, but it does not need access to metric storage. In the event-collector design discussed earlier, `FireView` could measure the raw duration and emit a `paintCompleted(duration)` observation. The application-level collector would decide whether to record it.

So the metric was reasonable and useful. Dropping it simplified the specified channel set, but it also removed the only direct measurement of a known, potentially expensive part of the current rendering path.

> Let's sketch application-owned event collector FrameMetricsCollector based on signals. When designing we need to answer these questions:
>
> - Who owns it?
> - What interface would it have?
> - If going with design based on signals, how will FireEngine report its part given that it's currently qt-free
> - Does it support on/off during runtime (off state mean no additional work is done)
>
> Also:
>
> - use `paintInterval` name
> - re-introduce `paintDuration` metric

## Proposed design

`FrameMetricsCollector` should be an application-level `QObject` that receives measurement events through direct Qt signals and owns all rolling metric storage.

```text
FireEngine
  advance() returns optional stage timings
       |
       v
FireController -- wakeMeasured / frameMeasured --+
                                                  |
FireView ------- paintMeasured -------------------+--> FrameMetricsCollector --> StatsPanel
                                                  |
                     enabledChanged --------------+
                          |                     |
                          v                     v
                    FireController           FireView
                          |
                          v
                     FireEngine
```

This removes `FrameProfiler` from `FireEngine`, removes `IntervalMetric` from the `FireView` constructor, and keeps Qt out of the engine.

## Ownership

`MainWindow` should own the collector because it is the composition root where all three producers meet:

- `FireEngine` produces simulation and shade timings.
- `FireController` observes timer wakes.
- `FireView` observes painting.
- `StatsPanel` consumes the resulting statistics.

The collector belongs to one application/window session, rather than to one engine or to the process globally.

Conceptually:

```cpp
FrameMetricsCollector
FireController
FireView
StatsPanel
```

are siblings composed by `MainWindow`.

This gives:

- One collector per running rendering session.
- Deterministic test isolation.
- No singleton/global state.
- A natural place for enable/disable policy.
- Metrics that can survive or be cleared independently of `FireEngine::reset()`.

## Collected channels

The collector would privately own five metrics:

```cpp
TimeSeriesMetric simulateDuration;
TimeSeriesMetric shadeDuration;
IntervalMetric wakeInterval;
TimeSeriesMetric paintDuration;
IntervalMetric paintInterval;
```

`presentInterval` should be renamed to `paintInterval`, because it measures Qt paint cadence rather than actual display presentation.

Definitions:

- `simulateDuration`: all simulation ticks executed for one produced frame.
- `shadeDuration`: heat-to-`PixelBuffer` conversion.
- `wakeInterval`: timer-wake start to timer-wake start.
- `paintDuration`: immediately before `QPainter` construction through immediately after its destruction.
- `paintInterval`: `paintEvent` start to subsequent `paintEvent` start.

`paintDuration` should include every paint, including resize, expose, and paused-state paints. That makes the known repeated-upscale cost visible.

## Collector interface

The public channels should disappear. Producers submit observations through methods, while readers receive a value snapshot.

A plausible interface is:

```cpp
struct PaintMeasurement {
    std::chrono::steady_clock::time_point startedAt;
    std::chrono::steady_clock::duration duration;
};

struct FrameMetricsSnapshot {
    bool enabled{false};
    MetricStatistics simulateDuration;
    MetricStatistics shadeDuration;
    MetricStatistics wakeInterval;
    MetricStatistics paintDuration;
    MetricStatistics paintInterval;
    std::optional<FrameReport> latestFrame;
};

class FrameMetricsCollector final : public QObject {
    Q_OBJECT

public:
    explicit FrameMetricsCollector(QObject* parent = nullptr);

    [[nodiscard]] bool isEnabled() const noexcept;
    [[nodiscard]] FrameMetricsSnapshot snapshot() const noexcept;

public slots:
    void setEnabled(bool enabled);
    void clear();

    void observeWake(std::chrono::steady_clock::time_point now);
    void observeFrame(FrameReport report);
    void observePaint(PaintMeasurement measurement);

signals:
    void enabledChanged(bool enabled);

private:
    // Five private metric channels and latest report.
};
```

The exact slots could instead accept a smaller `FrameStageTimings` value rather than the complete `FrameReport`. Accepting `FrameReport` is useful because it preserves correlation with tick count, discarded time, and frame index.

`StatsPanel` continues polling `snapshot()` at approximately 4 Hz. It never receives mutable access to the metric channels.

## How the Qt-free engine reports timings

`FireEngine` should not emit signals and should not know about `FrameMetricsCollector`.

Instead, extend its ordinary C++ result:

```cpp
struct FrameStageTimings {
    std::chrono::steady_clock::duration simulate;
    std::chrono::steady_clock::duration shade;
};

struct FrameReport {
    int ticksExecuted{0};
    Clock::duration elapsed{};
    Clock::duration discardedTime{};
    std::uint64_t frameIndex{0};
    std::optional<FrameStageTimings> stageTimings;
};
```

`std::optional` distinguishes “profiling was disabled” from a legitimately tiny or zero duration and performs no allocation.

The engine gets a pure-C++ switch:

```cpp
void setStageTimingEnabled(bool enabled) noexcept;
```

When enabled, `advance()` measures simulation and shading and returns them in `stageTimings`. When disabled, it performs those stages normally but does not call `Clock::now()` for profiling.

`FireController` translates the result into Qt events:

```cpp
void wakeMeasured(Clock::time_point now);
void frameMeasured(FrameReport report);
```

These signals are emitted only while metrics are enabled. The existing `frameReady` signal remains responsible for presentation and is emitted regardless of profiling state.

Thus, Qt remains entirely within `app/`:

```text
FireEngine:      returns a C++ value
FireController:  converts that value into a Qt signal
Collector:       consumes the signal
```

## Paint reporting

`FireView` gains `Q_OBJECT` and an instrumentation switch, but no metric dependency:

```cpp
void setMetricsEnabled(bool enabled) noexcept;

signals:
    void paintMeasured(PaintMeasurement measurement);
```

The enabled path is conceptually:

```cpp
const auto startedAt = Clock::now();
{
    QPainter painter(this);
    // fill, configure and draw
}
const auto duration = Clock::now() - startedAt;
emit paintMeasured(PaintMeasurement{startedAt, duration});
```

The collector uses:

- `measurement.duration` for `paintDuration`.
- `measurement.startedAt` to mark `paintInterval`.

Using start-to-start intervals cleanly separates scheduling cadence from the time spent inside each paint.

## Runtime on/off behavior

`FrameMetricsCollector::enabledChanged` is connected to:

```text
FireController::setMetricsEnabled
FireView::setMetricsEnabled
StatsPanel::setMetricsEnabled
```

`FireController::setMetricsEnabled` forwards the state to `FireEngine::setStageTimingEnabled`.

When disabled:

- `FireEngine` does not take simulation or shade timestamps.
- `FireController` does not emit measurement-specific signals.
- `FireView` does not take paint timestamps or emit `paintMeasured`.
- No ring buffers are updated.
- No statistics are calculated.
- The StatsPanel refresh timer is stopped.

The unavoidable cost is one predictable boolean branch at each instrumentation site. A runtime-switchable feature cannot literally cost zero instructions because the code must determine whether it is enabled. The meaningful guarantee is:

> When disabled, there are no profiling clock reads, measurement signal emissions, metric writes, sorting, snapshot computation, or panel refreshes.

On transition from disabled to enabled, the collector should start a fresh measurement session:

1. Clear duration samples.
2. Clear interval baselines.
3. Mark itself enabled.
4. Emit `enabledChanged(true)`.

This prevents the disabled period from appearing as one enormous wake or paint interval.

Whether disabling immediately clears the previous snapshot is a UI choice. I would retain the last snapshot internally but have `snapshot().enabled == false`, allowing the panel to show either “Disabled” or the frozen previous values.

## Signal wiring

The composition root would establish:

```cpp
collector.enabledChanged -> controller.setMetricsEnabled
collector.enabledChanged -> fireView.setMetricsEnabled
collector.enabledChanged -> statsPanel.setMetricsEnabled

controller.wakeMeasured   -> collector.observeWake
controller.frameMeasured  -> collector.observeFrame
fireView.paintMeasured    -> collector.observePaint
```

All connections are direct and same-thread, so the observation structures do not initially require Qt metatype registration. They would need registration if any connection later became queued.

## Resulting boundaries

- `FireEngine`: performs work and optionally reports raw stage durations.
- `FireController`: adapts engine results and timer events to Qt signals.
- `FireView`: paints and optionally reports raw paint measurements.
- `FrameMetricsCollector`: owns aggregation, enablement policy, and snapshots.
- `StatsPanel`: displays snapshots.
- `MainWindow`: owns and wires the session.

This design removes the current mutable profiler exposure, avoids a singleton, supports runtime disable with effectively no measurement work, uses the more accurate `paintInterval` name, and restores `paintDuration` without passing metric-storage objects into widgets.

> I want you to implement `FrameMetricsCollector` according to this design and re-wire related classes - `FireEngine`, `FireController`, `FireView`. Do not introduce `StatsPanel` just yet.

<details><summary>8 previous messages</summary>

> > 🤝 Following project rules.
>
> I’ll replace the engine-owned metric aggregate with a `MainWindow`-owned signal collector, add optional Qt-free stage timings to `FrameReport`, gate all measurement work at runtime, restore `paintDuration`, and rename the interval to `paintInterval`. I’ll also update the headless engine tests and add focused collector tests, without introducing `StatsPanel`.
>
> <details><summary>Created 3 files, edited 3 files, ran a command</summary>
>
> - Ran `git status --short
> sed -n '1,240p' CMakeLists.txt
> sed -n '1,240p' tests/metrics/MetricsTests.cpp
> sed -n '1,260p' tests/engine/FrameClockTests.cpp
> sed -n '1,220p' src/engine/FrameReport.hpp
> sed -n '1,240p' src/engine/FireEngine.hpp
> sed -n '1,260p' src/engine/FireEngine.cpp
> sed -n '1,220p' src/app/FireController.hpp
> sed -n '1,260p' src/app/FireController.cpp
> sed -n '1,180p' src/app/FireView.hpp
> sed -n '1,240p' src/app/FireView.cpp
> sed -n '1,200p' src/app/MainWindow.cpp`
> - Wrote `./src/engine/FireEngine.cpp` (+22 -12)
> - Wrote `./src/engine/FireEngine.hpp` (+5 -4)
> - Wrote `./src/engine/FrameReport.hpp` (+7 -0)
> - Wrote `./src/app/FrameMetricsCollector.cpp` (+65 -0)
> - Wrote `./src/app/FrameMetricsCollector.hpp` (+52 -0)
> - Wrote `./src/app/PaintMeasurement.hpp` (+8 -0)
>
> </details>
>
> The collector and Qt-free timing payload are defined. The key runtime behavior is now explicit: the engine returns `std::optional` stage timings, while the controller and view emit dedicated measurement signals only when enabled; disabled mode performs no profiling clock reads or metric writes beyond the unavoidable enablement branch.
>
> <details><summary>Created a file, edited 10 files, deleted 2 files, ran 4 commands</summary>
>
> - Wrote `./src/app/FireController.cpp` (+11 -1)
> - Wrote `./src/app/FireController.hpp` (+4 -2)
> - Wrote `./src/app/FireView.cpp` (+13 -8)
> - Wrote `./src/app/FireView.hpp` (+11 -3)
> - Wrote `./src/app/MainWindow.cpp` (+25 -1)
> - Wrote `./src/app/PaintMeasurement.hpp` (+4 -2)
> - Wrote `./CMakeLists.txt` (+23 -3)
> - Deleted `./src/metrics/FrameProfiler.cpp` (+0 -8)
> - Deleted `./src/metrics/FrameProfiler.hpp` (+0 -13)
> - Wrote `./tests/app/FrameMetricsCollectorTests.cpp` (+136 -0)
> - Wrote `./tests/metrics/MetricsTests.cpp` (+0 -23)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+18 -18)
> - Ran `git diff --check
> rg -n "FrameProfiler|presentInterval|profiler\(\)|frameProfiler" CMakeLists.txt src tests docs/render_pipeline_redesign.md || true
> rg -n "paintInterval|paintDuration|stageTimings|MetricsEnabled|Measured" src tests | sort
> git status --short`
> - Ran `cmake --preset cfg-release && cmake --build --preset build-release`
> - Wrote `./src/engine/FireEngine.cpp` (+1 -1)
> - Wrote `./tests/metrics/MetricsTests.cpp` (+1 -0)
> - Ran `cmake --build --preset build-release`
> - Ran `ctest --test-dir build/release --output-on-failure`
>
> </details>
>
> Release compilation and all five test targets pass, including the new collector tests. The ownership and signal path now work without exposing metric storage. I’m tightening the result by removing the obsolete engine-owned profiling helpers and reconciling the architecture document with this revised design before sanitizer validation.
>
> <details><summary>Edited 3 files, deleted 2 files, explored 12 files, 2 searches, ran 4 commands</summary>
>
> - Wrote `./CMakeLists.txt` (+0 -2)
> - Deleted `./src/metrics/ScopedTimer.cpp` (+0 -6)
> - Deleted `./src/metrics/ScopedTimer.hpp` (+0 -16)
> - Wrote `./tests/metrics/MetricsTests.cpp` (+0 -23)
>
> <details><summary>Explored 5 files</summary>
>
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
>
> </details>
>
> - Wrote `./docs/render_pipeline_redesign.md` (+29 -19)
>
> - Wrote `./docs/render_pipeline_redesign.md` (+11 -35)
>
> - Wrote `./docs/render_pipeline_redesign.md` (+22 -15)
>
> - Wrote `./docs/render_pipeline_redesign.md` (+15 -7)
>
> - Wrote `./docs/render_pipeline_redesign.md` (+3 -3)
>
> - Wrote `./docs/render_pipeline_redesign.md` (+14 -0)
>
> - Wrote `./docs/render_pipeline_redesign.md` (+9 -6)
>
> <details><summary>Explored 2 files, a search</summary>
>
> - Searched for `FrameProfiler|presentInterval|ScopedTimer|engine\.profiler|four channels|no fifth|profiler\.` in `render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
> - Read `./docs/render_pipeline_redesign.md`
>
> </details>
>
> - Wrote `./docs/render_pipeline_redesign.md` (+11 -5)
>
> <details><summary>Explored a search</summary>
>
> - Searched for `FrameProfiler|presentInterval|ScopedTimer|engine\.profiler|four channels|no fifth|profiler\.|not yet implemented|FrameMetricsCollector|paintDuration|paintInterval` in `render_pipeline_redesign.md`
>
> </details>
>
> - Ran `rg -n "FrameProfiler|ScopedTimer|presentInterval|profiler\(\)|frameProfiler" CMakeLists.txt src tests || true
> git diff --check
> git diff --stat
> git status --short`
>
> <details><summary>Explored 12 files</summary>
>
> - Read `./src/app/FrameMetricsCollector.hpp`
> - Read `./src/app/FrameMetricsCollector.cpp`
> - Read `./src/app/PaintMeasurement.hpp`
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/engine/FireEngine.cpp`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./CMakeLists.txt`
> - Read `./CMakeLists.txt`
>
> </details>
>
> - Ran `cmake --preset cfg-release && cmake --build --preset build-release && ctest --test-dir build/release --output-on-failure`
>
> - Ran `git diff -- docs/render_pipeline_redesign.md`
>
> - Ran `cmake --preset cfg-sanitize && cmake --build --preset build-sanitize && ctest --test-dir build/sanitize --output-on-failure`
>
> </details>
>
> Release and ASan/UBSan builds now pass all five tests. The revised dependency graph is also enforced: `fire_engine` no longer links `fire_metrics`; only the Qt application owns and aggregates measurements. I’m smoke-testing the live signal path, then I’ll perform the required final formatting pass.
>
> <details><summary>Edited 3 files, ran 4 commands</summary>
>
> - Ran `ASAN_OPTIONS=halt_on_error=1 UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 QT_QPA_PLATFORM=offscreen ./build/sanitize/classic_fire.app/Contents/MacOS/classic_fire`
> - Wrote `./src/app/FireView.cpp` (+15 -11)
> - Wrote `./src/app/FireView.hpp` (+2 -0)
> - Wrote `./src/app/PaintMeasurement.hpp` (+1 -1)
> - Ran `cmake --build --preset build-release && ctest --test-dir build/release --output-on-failure && cmake --build --preset build-sanitize && ctest --test-dir build/sanitize --output-on-failure`
> - Ran `clang-format -i src/app/FireController.cpp src/app/FireController.hpp src/app/FrameMetricsCollector.cpp src/app/FrameMetricsCollector.hpp src/app/FireView.cpp src/app/FireView.hpp src/app/MainWindow.cpp src/app/PaintMeasurement.hpp src/engine/FireEngine.cpp src/engine/FireEngine.hpp src/engine/FrameReport.hpp tests/app/FrameMetricsCollectorTests.cpp tests/engine/FrameClockTests.cpp tests/metrics/MetricsTests.cpp`
> - Ran `git diff HEAD --check
> git status --short
> git diff HEAD --stat
> rg -n "FrameProfiler|ScopedTimer|presentInterval|profiler\(\)|frameProfiler" CMakeLists.txt src tests docs/render_pipeline_redesign.md || true
> rg -n "#include <Q|#include \"Q|Qt::|QObject|QWidget" src/engine src/render src/sim src/metrics || true`
>
> </details>
</details>

Implemented the application-owned signal-based metrics collector without adding `StatsPanel`.

Key changes:

- Added [`FrameMetricsCollector`](./src/app/FrameMetricsCollector.hpp), owned and wired by [`MainWindow`](./src/app/MainWindow.cpp).
- Collector privately aggregates:
  - `simulateDuration`
  - `shadeDuration`
  - `wakeInterval`
  - `paintDuration`
  - `paintInterval`
- [`FireEngine`](./src/engine/FireEngine.hpp) remains Qt-free and returns optional stage timings through [`FrameReport`](./src/engine/FrameReport.hpp).
- [`FireController`](./src/app/FireController.hpp) translates engine and wake observations into gated Qt signals.
- [`FireView`](./src/app/FireView.hpp) no longer accepts metric storage. It emits a gated paint measurement covering the complete `QPainter` lifetime.
- Runtime disablement avoids profiling clock reads, measurement signals, metric writes, and statistics work; only enablement branches remain.
- Re-enabling starts a fresh measurement session.
- Removed obsolete `FrameProfiler` and `ScopedTimer`.
- Updated the architecture document to reflect the revised design.
- Added dedicated collector tests.

Validation:

- Release build: passed
- Sanitizer build: passed
- All 5 test targets: passed
- Sanitized offscreen Qt smoke test: passed
- Final `clang-format` pass completed.

> What was the reason to introduce PaintMeasurement type, it looks escessive

`PaintMeasurement` was introduced because one paint event supplies two related facts:

- `startedAt` → used to calculate `paintInterval`
- `duration` → recorded as `paintDuration`

Bundling them ensures they travel together as one observation. However, with only one producer, one consumer, and two fields, I agree the type is excessive for this project.

A simpler signal is sufficient:

```cpp
void paintMeasured(
    std::chrono::steady_clock::time_point startedAt,
    std::chrono::steady_clock::duration duration);
```

with a matching collector slot. It preserves the exact measurement semantics without introducing another domain type. `PaintMeasurement` would become worthwhile only if paint observations gained additional data such as frame index, destination size, or device-pixel ratio.

No code was changed.

> we currently have `using Clock = std::chrono::steady_clock;` defined multiple time inside multiple metrics related type. It looks to me like a problem. I would suggest creating a single alias MetricsClock and share it instead of defining internal clocks. Any reason not to do it this way?&#x20;

<details><summary>2 previous messages</summary>

> I’ll inventory the current clock aliases and their boundaries first, because a shared alias is helpful only if every occurrence represents the same timing domain rather than unrelated implementation convenience.
>
> <details><summary>Explored a search</summary>
>
> - Searched for `using Clock|steady_clock` in `src`
>
> </details>
</details>

A shared alias is reasonable within the metrics subsystem, but I would not replace every `steady_clock` reference with `MetricsClock`.

The current aliases represent two distinct concerns:

- `TimeSeriesMetric`, `IntervalMetric`, paint measurements, and the collector use a clock for performance observation.
- `FireController`, `FrameClock`, and `FireEngine` use `steady_clock` for frame-loop progression and elapsed simulation time.

Calling the second category `MetricsClock` would make the engine appear dependent on instrumentation again, undermining the separation we just introduced.

Also, repeated aliases are not separate types:

```cpp
using Clock = std::chrono::steady_clock;
```

Every occurrence is already exactly the same type. A shared alias documents policy and centralizes spelling, but it does not add type safety.

My recommendation would be:

```cpp
// metrics/MetricsClock.hpp
#pragma once

#include <chrono>

using MetricsClock = std::chrono::steady_clock;
```

Use it in:

- `TimeSeriesMetric`
- `IntervalMetric`
- `FrameMetricsCollector`
- `FireView`’s paint measurements

Keep engine timing expressed independently:

- `FireController::Clock`
- `FrameClock`
- `FrameReport`
- `FireEngine`

Because `MetricsClock` aliases `steady_clock`, controller timestamps and engine durations remain directly compatible with collector slots.

Once `PaintMeasurement` is removed, this would eliminate most of the confusing repetition without making the Qt-free engine conceptually depend on the metrics subsystem.

So: **yes to a shared `MetricsClock`, scoped to metrics collection; no to using it as the universal application clock.** No code was changed.

> That was exactly my idea. Drop `PaintMeasurement` and switch to a shared `MetricsClock`, scoped to metrics collection

<details><summary>9 previous messages</summary>

> I’ll remove the paint DTO, introduce `MetricsClock` in the pure metrics module, and use it directly across metric storage, collector slots, paint signals, and related tests. Frame-loop timing in `engine/` and `FireController` will remain independently expressed as `steady_clock`.
>
> <details><summary>Created a file, edited 12 files, deleted a file, ran 2 commands</summary>
>
> - Wrote `./src/app/FireView.cpp` (+2 -2)
> - Wrote `./src/app/FireView.hpp` (+2 -2)
> - Wrote `./src/app/FrameMetricsCollector.cpp` (+5 -4)
> - Wrote `./src/app/FrameMetricsCollector.hpp` (+3 -3)
> - Deleted `./src/app/PaintMeasurement.hpp` (+0 -10)
> - Wrote `./src/metrics/IntervalMetric.cpp` (+1 -1)
> - Wrote `./src/metrics/IntervalMetric.hpp` (+3 -4)
> - Wrote `./src/metrics/MetricsClock.hpp` (+5 -0)
> - Wrote `./src/metrics/TimeSeriesMetric.cpp` (+3 -3)
> - Wrote `./src/metrics/TimeSeriesMetric.hpp` (+3 -4)
> - Wrote `./CMakeLists.txt` (+1 -2)
> - Wrote `./docs/render_pipeline_redesign.md` (+5 -9)
> - Wrote `./tests/app/FrameMetricsCollectorTests.cpp` (+8 -8)
> - Wrote `./tests/metrics/MetricsTests.cpp` (+1 -1)
> - Ran `rg -n "PaintMeasurement|using Clock|TimeSeriesMetric::Clock|IntervalMetric::Clock|steady_clock" src tests docs/render_pipeline_redesign.md | sort
> sed -n '1,100p' src/metrics/TimeSeriesMetric.hpp
> sed -n '1,100p' src/metrics/IntervalMetric.hpp
> sed -n '1,100p' src/app/FrameMetricsCollector.hpp
> sed -n '1,90p' src/app/FireView.hpp
> git diff --check`
> - Wrote `./docs/render_pipeline_redesign.md` (+7 -0)
> - Wrote `./src/app/FrameMetricsCollector.hpp` (+0 -1)
> - Wrote `./src/metrics/TimeSeriesMetric.hpp` (+0 -1)
> - Ran `cmake --preset cfg-release && cmake --build --preset build-release && ctest --test-dir build/release --output-on-failure`
>
> </details>
>
> Context compaction
>
> Source: automatic
> Status: completed
>
> The shared alias is in place and the release build plus all five tests pass. I’m validating the same change under sanitizers now, then I’ll format only the affected C++ files and do a final stale-reference check.
>
> <details><summary>Ran a command</summary>
>
> - Ran `cmake --preset cfg-sanitize && cmake --build --preset build-sanitize && ctest --test-dir build/sanitize --output-on-failure`
>
> </details>
>
> Sanitizer build and all tests pass as well. The Qt signal/slot signature using `MetricsClock` is accepted by MOC; I’m now checking that `PaintMeasurement` and metric-local clock aliases are fully gone before the final formatting pass.
>
> <details><summary>Ran a command</summary>
>
> - Ran `rg -n "PaintMeasurement|TimeSeriesMetric::Clock|IntervalMetric::Clock|using Clock" CMakeLists.txt src tests docs/render_pipeline_redesign.md || true
> git diff --check`
>
> </details>
>
> The metrics layer has no remaining `PaintMeasurement` or per-metric `Clock` aliases. The only `using Clock` left is `FireController`’s private scheduling clock, which is outside metrics collection and should remain decoupled from instrumentation. I’m applying the required final formatting now.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `clang-format -i src/metrics/MetricsClock.hpp src/metrics/TimeSeriesMetric.hpp src/metrics/TimeSeriesMetric.cpp src/metrics/IntervalMetric.hpp src/metrics/IntervalMetric.cpp src/app/FrameMetricsCollector.hpp src/app/FrameMetricsCollector.cpp src/app/FireView.hpp src/app/FireView.cpp tests/metrics/MetricsTests.cpp tests/app/FrameMetricsCollectorTests.cpp`
> - Ran `git diff --check
> git status --short
> rg -n "PaintMeasurement|TimeSeriesMetric::Clock|IntervalMetric::Clock" CMakeLists.txt src tests docs/render_pipeline_redesign.md || true
> git diff --stat HEAD`
>
> </details>
</details>

Implemented.

- Added shared [`MetricsClock`](./src/metrics/MetricsClock.hpp) for metrics collection.
- Updated `TimeSeriesMetric`, `IntervalMetric`, `FrameMetricsCollector`, and `FireView` to use it.
- Removed `PaintMeasurement`; `paintMeasured` now sends `startedAt` and `duration` directly.
- Kept `FireController`’s private scheduling clock separate because it controls frame scheduling, not metrics collection.
- Updated tests, CMake, and architecture documentation.

Validation:

- Release: 5/5 tests passed.
- Sanitizers: 5/5 tests passed.
- `git diff --check` passed.
- Modified C++ files formatted with the repository’s `clang-format` configuration.
