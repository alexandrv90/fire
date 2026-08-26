# Render Pipeline Redesign

**Status:** In progress — migration steps 1–7 implemented
**Date:** 2026-08-26
**Applies to:** the whole application; the rendering path in particular

This document proposes a decomposition that separates the frame loop, the simulation, the
rendering, and the Qt shell into modules with explicit contracts. It exists because the
current structure has no seam for anything that is neither "the simulation" nor "a widget" —
performance monitoring being the concrete case that exposed it.

**Scope: structure and observability, not frame pacing.** The frame pacing algorithm is kept
exactly as it is today — a `QTimer` wake driving a fixed-timestep accumulator, a single pixel
buffer, and a CPU upscale performed on every paint. This redesign moves that algorithm into
`FrameClock` and `FireEngine` unchanged; it does not alter when frames are produced, how
simulation time maps to display time, or how pixels reach the screen. Nothing here makes the
output smoother. What it does is make the existing behaviour measurable, testable and
separable, which is the prerequisite for changing it later. See decision 6.6 and section 8.

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
|   FrameMetricsCollector . StatsPanel                               |
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
|   MetricsClock . TimeSeriesMetric . IntervalMetric . Statistics   |
+--------------------------------------------------------------------+
```

**Dependency rule:** `app -> {engine, metrics}`, `engine -> {sim, render}`, and
`render -> sim` (the renderer consumes `HeatFrame`). `sim` and `metrics` depend on nothing.
Only the executable links Qt.

CMake targets:

| Target | Links | Qt |
|--------|-------|----|
| `fire_metrics` | — | no |
| `fire_simulation` | — | no |
| `fire_render` | `fire_simulation` | no |
| `fire_engine` | `fire_simulation`, `fire_render` | no |
| `classic_fire` | `fire_engine`, `fire_metrics`, `Qt::Widgets` | yes |

`FireEngine` returns optional raw stage durations in `FrameReport`; aggregation belongs to
the application-owned `FrameMetricsCollector`. Consequently neither `fire_engine` nor
`fire_render` links `fire_metrics`.

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
    if metrics enabled: emit wakeMeasured(now)
  |
  v FireEngine::advance(elapsed) ---------------------> FrameReport
      |
      +-- FrameClock::consume(elapsed) -> TickPlan{ticks, discardedTime}
      |
      +-- if plan.ticks == 0: return report unshaded
      |
      +-- [Simulate]  N x FireSimulation::tick()          <-- optionally timed
      |
      +-- [Extract]   HeatFrame frame = simulation.heat()
      |
      +-- [Shade]     FireRenderer::render(frame)         <-- optionally timed
  |
  +-- if report.stageTimings: emit frameMeasured(FrameReport)
  |
  v if report.ticksExecuted > 0: emit frameReady(FrameReport)
      |
      +--> FireView::present(engine.frame())
      |      copy PixelBuffer into the view's own QImage -> update()
      |      paintEvent: [Paint] QPainter construction through destruction
      |                  if metrics enabled: emit paintMeasured(startedAt, duration)
      |
      +--> FrameMetricsCollector receives wakeMeasured, frameMeasured and paintMeasured
             |
             +--> StatsPanel reads snapshot()             (throttled, see 9)
```

**Zero-tick wakes are not shaded.** A 16 ms wake against a 16.667 ms step means roughly one
wake in twenty-five drains no ticks. Shading unchanged heat produces byte-identical pixels,
costs a full conversion pass, and would contaminate the Shade statistics with work that does
not exist. The current code carries this guard as `if (ticksExecuted > 0)` in
`FireController::advanceFrame`; the redesign keeps it, moved into the engine where the
decision is made.

Three timed **durations**: **Simulate** and **Shade**, optionally measured inside
`FireEngine::advance`, and **Paint**, measured by `FireView` from immediately before
`QPainter` construction through immediately after its destruction. Extract is a view
construction and costs nothing worth measuring. The `PixelBuffer`-to-`QImage` copy remains
unmeasured because it is expected to be negligible at the current frame size.

Two timed **intervals**: **wake interval**, wake start to wake start, and **paint interval**,
`paintEvent` start to `paintEvent` start. `FrameMetricsCollector` records both from timestamps
carried by producer signals.

The distinction between the two kinds is the point. Stage durations answer "is a frame
expensive?"; intervals answer "do frames arrive evenly?", which is what a viewer actually
perceives. A pipeline whose stages sum to three milliseconds can still judder, and only the
interval metrics show it — durations alone would make the panel look healthy while the fire
stutters. Paint duration distinguishes expensive Qt scaling and blitting from irregular paint
scheduling. Given that this redesign deliberately does not change pacing (decision 6.6), the
interval metrics are the ones that make the carried-over behaviour visible.

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
using MetricsClock = std::chrono::steady_clock;

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
    void record(MetricsClock::duration duration) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;
};

// Records the gap between successive occurrences of a recurring event rather than the cost
// of a scope. The first mark after construction or clear() establishes the reference and
// records no sample.
class IntervalMetric final {
public:
    void mark(MetricsClock::time_point now) noexcept;
    void clear() noexcept;

    [[nodiscard]] MetricStatistics statistics() const noexcept;
};

```

Both metric types record into a fixed-size ring buffer, so recording never allocates.
`statistics()` computes on demand and returns a value, so reading does not allocate either.
They remain generic storage types; they do not name application channels or own enablement
policy.

`MetricsClock` centralizes the monotonic clock used by metric storage and application-side
measurement producers. Frame pacing remains independently expressed with
`std::chrono::steady_clock` in `engine/`; the alias does not create an engine-to-metrics
dependency.

The application-specific channels are private members of `FrameMetricsCollector`: simulate
duration, shade duration, wake interval, paint duration and paint interval. Readers receive a
fixed `FrameMetricsSnapshot` value. A registration table was considered and rejected because
there are five known channels and no runtime registration requirement; a fixed snapshot is
allocation-free and makes the display contract explicit.

The collector is application-owned rather than global. A singleton would make call sites
shorter but would introduce hidden mutable state, merge independent engine sessions and make
tests depend on global resets. `MainWindow` owns one collector for the rendering session and
wires producers to it with direct Qt signals.

**Counters are not included.** A `CounterMetric` (total ticks, frames that discarded time) was
specified in an earlier draft with no consumer. By the standard applied above it should not
exist until `StatsPanel` displays one. See section 9.

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

// One control point of the colour ramp. Stops are ordered by index; the first must be 0 and
// the last 255, and entries between two stops are linearly interpolated.
struct PaletteStop {
    std::uint8_t index;
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

struct FrameStageTimings {
    std::chrono::steady_clock::duration simulateDuration{};
    std::chrono::steady_clock::duration shadeDuration{};
};

struct FrameReport {
    int ticksExecuted{0};                                  // zero: nothing simulated, nothing shaded
    std::chrono::steady_clock::duration elapsed{};         // wall time this wake accounted for
    std::chrono::steady_clock::duration discardedTime{};   // non-zero: could not keep up
    std::uint64_t frameIndex{0};
    std::optional<FrameStageTimings> stageTimings;          // absent: collection disabled or no frame
};

// engine/FireEngine.hpp

// The frame loop and the owner of the long-lived pure-C++ simulation and rendering resources.
class FireEngine final {
public:
    FireEngine(std::size_t simulationWidth, std::size_t simulationHeight);

    FrameReport advance(std::chrono::steady_clock::duration elapsed);
    void reset() noexcept;

    [[nodiscard]] const PixelBuffer& frame() const noexcept;

    [[nodiscard]] const FireParameters& parameters() const noexcept;
    void setParameters(const FireParameters& parameters) noexcept;

    void setStageTimingEnabled(bool enabled) noexcept;

private:
    FireSimulation simulation;
    FireRenderer renderer;
    FrameClock clock;
    std::uint64_t frameIndex{0};
    bool stageTimingEnabled{false};
};
```

`advance` is the whole loop:

```cpp
FrameReport FireEngine::advance(const std::chrono::steady_clock::duration elapsed) {
    const TickPlan plan = clock.consume(elapsed);
    if (plan.ticks == 0) {
        // Heat is unchanged, so shading would reproduce the existing pixels exactly. The
        // frame index does not advance: no frame was produced.
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
```

The zero-tick early return is behaviour carried over from
`FireController::advanceFrame`, not a new optimisation; section 3 explains why it matters and
decision 6.6 explains why the wakes that trigger it still exist.

`FireEngine` is testable headlessly: advance it sixty times with synthetic elapsed values and
assert on `frame()` and the reports. A zero-tick advance is directly assertable — feed it an
elapsed shorter than one step and require that `frame()` is untouched.

The engine remains Qt-free: it returns optional raw `std::chrono` durations rather than
emitting signals or writing into application-owned storage. When timing is disabled it takes
no profiling timestamps. The unavoidable runtime-switching overhead is one predictable
boolean branch.

`setParameters` taking a whole `FireParameters` value — rather than one setter per field —
resolves problems 5 and 6 together: two engine entry points regardless of knob count, and the
panel round-trips a value it can also read back.

### 4.5 `app/` — Qt shell

| Class | Responsibility |
|-------|----------------|
| `FireController` | Qt adapter only. Owns `QTimer` and `FireEngine`; run/pause/toggle/reset; emits frame, parameter and run-state signals plus gated wake/frame measurement signals. |
| `FireView` (was `FireWidget`) | Presentation surface only. `present(const PixelBuffer&)` copies into its own `QImage` and calls `update()`; `paintEvent` blits into `fitPreservingAspect(...)` and emits gated paint start and duration values. No metric storage, palette, conversion or simulation constants. |
| `FrameMetricsCollector` | Application-owned event collector. Owns five private rolling channels, runtime enablement and immutable snapshots; receives observations through direct Qt signals. |
| `ControlPanel` | Binds sliders to a `FireParameters` value; emits `parametersChanged(FireParameters)` on edits and accepts `setParameters(FireParameters)` for read-back. |
| `StatsPanel` | One fixed row per `FrameMetricsSnapshot` channel, plus the latest `FrameReport`. |
| `MainWindow` | Wiring only. Owns the rendering-session collector, owns the simulation dimensions as the composition root and wires producers to consumers. |

**Parameter binding terminates by signal blocking, not by comparison.** The panel emits
`parametersChanged` on a user edit; `FireController` calls `FireEngine::setParameters` and
then re-emits `parametersChanged` carrying the value the engine actually holds, which the
panel applies through `setParameters`. That value may legitimately differ from what the user
requested, because `FireParameters` clamps on write — so comparing sent against received
cannot be the loop breaker. `ControlPanel::setParameters` blocks its own widget signals for
the duration of the update instead. The same path carries parameter changes originating
elsewhere, such as `reset`, which is what keeps the sliders from going stale.

`FireView` receives no metric-storage dependency. When collection is enabled it measures from
immediately before `QPainter` construction until immediately after destruction and emits the
raw start time and duration. `FrameMetricsCollector` derives `paintInterval` from successive
start times and records `paintDuration` separately.

`FrameMetricsCollector::enabledChanged(bool)` is connected to both producers. Disabled mode
takes no profiling-only clock readings, emits no measurement signals, writes no samples and
performs no statistics work. A runtime switch necessarily retains a boolean branch at each
instrumentation site. Re-enabling starts a fresh measurement session so disabled time cannot
appear as one large interval.

`StatsPanel` refreshes on a timer of its own rather than on every `frameReady`. Its numbers
are rolling statistics over the last N samples; recomputing sixty labels a second makes them
unreadable without making them more accurate. See section 9.

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
`QImage::Format_RGB32`, so `present` is a straight `std::memcpy` into a `QImage` of the same
geometry with no per-pixel conversion at the boundary.

**Lifetime.** `FireView` owns its `QImage` and copies the engine's `PixelBuffer` into it on
`present`, so the view holds no pointer into the engine and the two have independent
lifetimes. A borrowing `QImage(const uchar*, int, int, Format)` was the alternative: it avoids
the copy, but only stays correct while Qt destroys the widget subtree before the controller
that owns the engine — an ordering rule that lives in the composition root, is invisible at
both use sites, and that no future edit is obliged to remember. The copy is one frame, a few
hundred kilobytes at the current simulation size and on the order of ten microseconds at
60 Hz, and it converts that rule into an invariant that cannot be broken. Given that this
project builds under ASan specifically to catch use-after-free, encoding one deliberately
would be a poor trade.

**Single-threaded rendering.** `QWidget::update()` posts a deferred paint request, so
`paintEvent` never runs nested inside the timer slot, and `FireEngine::advance` spins no
nested event loop. Rendering and painting are therefore strictly serialized by the event loop
and a single buffer is safe. This assumption is why there is no double buffering; it holds
only while nothing calls synchronous `repaint()` from the render path.

The same deferral has a cost, carried over unchanged from the current implementation:
`update()` coalesces, so when two wakes shade before the compositor paints, the first shaded
frame is discarded work. Shading runs on the timer clock and painting runs on the
compositor's, and this redesign does not change that. See decision 6.6.

---

## 6. Design decisions

Recorded so they are not silently re-litigated.

### 6.1 No `RenderPass` / `RenderContext` / `RenderPipeline` — rejected

A pass interface with a linear pipeline was considered and rejected. Its main argument was
that self-identifying stages let the pipeline register profiler channels automatically, so
instrumentation is not hand-written at each site. That argument dissolves once `FireEngine`
exists: the engine is a single orchestration point that runs every pure-C++ stage, so two
explicit timing boundaries in one function give the same property without a vtable, an owned
`vector<unique_ptr<RenderPass>>`, or a per-frame context struct.

Runtime pass ordering and polymorphic extension were the other arguments, and both are
speculative — there is exactly one pass.

**Cost of the decision:** adding a post-process effect later means editing
`FireRenderer::render` rather than adding one registration line; per-effect timing needs a
second explicit timing boundary inside `render()`.

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
dead weight. `FrameClock` can compute it in one line when something needs it — which is to say
when pacing is revisited, since interpolation is a pacing change and is held under decision
6.6.

### 6.5 `render/` is Qt-free rather than QImage-based — accepted with a cost

Keeping the renderer free of the windowing system is the load-bearing separation in
real-time graphics and it makes the entire pixel path unit-testable. The cost is the copy at
the Qt boundary described in section 5. Letting `FireRenderer` own a `QImage` directly would
be a defensible simplification that trades that testability away.

### 6.6 Frame pacing carried over unchanged — accepted with known costs

The pacing algorithm is held exactly as it is: a `QTimer` wake at approximately 16 ms, a fixed
60 Hz accumulator with a three-tick catch-up clamp, a single pixel buffer, and a CPU bilinear
upscale performed on every paint. `FrameClock` and `FireEngine` are a relocation of that
algorithm, not a revision of it.

This is a deliberate hold. Restructuring and re-pacing at the same time makes it impossible to
attribute any observed change to either, and the interval metrics introduced here are what
would establish the baseline a pacing change should be judged against. Measure first.

**Known costs carried forward, recorded so they are not later rediscovered as bugs:**

- The 16 ms wake and the 16.667 ms step beat against each other, so roughly one wake in
  twenty-five drains no ticks and the displayed frame repeats. The timer's phase relative to
  display scanout also drifts continuously; a `QTimer` cannot be aligned to refresh. The
  `wakeInterval` metric and `FrameReport::ticksExecuted` make this visible.
- Shading runs on the timer clock while painting runs on the compositor's, so a shade whose
  paint request is coalesced into a later one is discarded work. Moving shading to paint time
  would fix it and is a pacing change.
- The upscale in `paintEvent` re-runs on every repaint, including resizes, expose events and
  repaints while paused, where the pixels have not changed. Nothing caches the scaled result.

None of these are introduced by this redesign and none are fixed by it. `FrameClock` is where
a future pacing change lands: it already returns `discardedTime`, and decision 6.4 records
where an interpolation alpha would go.

### 6.7 Application-owned metrics collector rather than a singleton — accepted

The five metrics span three producers: engine stages, timer wakes and widget paints. Owning
their storage inside `FireEngine` misstates that scope and forces mutable metric objects
through `FireController` into `FireView`. A process-wide singleton removes constructor
parameters but replaces them with hidden mutable state, merges independent sessions and makes
tests responsible for global cleanup.

`MainWindow` therefore owns one `FrameMetricsCollector` for the rendering session. Qt-facing
producers send raw observations through direct signals; the Qt-free engine places optional raw
stage durations in `FrameReport`. The collector alone owns rolling storage and enablement
policy. Producers suppress timing and measurement signals while disabled, so the off path has
no measurement work beyond its required branch.

---

## 7. Migration plan

Each step builds, ships, and leaves the application working.

1. **`fire_metrics` plus a test target.** Pure addition, no callers. Establishes CTest, which
   is what makes every later step safe. Resolves problem 8 for this module.
2. **`FireSimulation` tests, against the implementation as it stands.** No production change.
   The fixed seed already makes the simulation reproducible, so a hash of the heat map after
   N ticks is a regression net for everything that follows — including the pacing behaviour
   held under decision 6.6, which must survive the move into `FrameClock` intact. Problem 8
   names the untested deterministic simulation specifically; this is the step that answers it.
3. **`HeatFrame`.** `FireSimulation::heat()` returns it instead of a bare span; `FireWidget`
   and `MainWindow` adapt. Resolves problem 4.
4. **`fire_render`: `PixelBuffer`, `FirePalette`, `FireRenderer`, `Viewport`, plus tests.** A
   new target with no callers yet, so the application is untouched.
5. **`FireWidget` becomes `FireView`.** It hands its palette and conversion code to
   `fire_render` and keeps only presentation, including the copy at `present`. Resolves
   problem 3.
6. **`FrameClock` plus tests.** `FireController` loses the accumulator. Resolves problem 2.
7. **`FireEngine` and `FrameMetricsCollector`.** `FireController` loses everything else and
   becomes a Qt adapter. The application-owned collector receives gated wake, stage and paint
   observations; `paintDuration` is restored and `paintInterval` names paint cadence
   accurately. Resolves problems 1 and 5.
8. **`ControlPanel` binds a `FireParameters` value.** Resolves problem 6.
9. **`StatsPanel`.** The feature that motivated the redesign; by this point it is an additive
   reader of `FrameMetricsCollector::snapshot()`.

Step 1 comes first specifically because it is the only pure addition and it delivers the test
target that every later step depends on. Step 2 comes second because a behaviour-preserving
restructure needs a behavioural test before it starts, not after.

Steps 3 through 5 are one earlier step split into three. Combined, it introduced a type, a
target, four classes and a widget rewrite at once, which is more than a step that claims to
build and ship on its own can carry.

Problem 7 is addressed incrementally as targets are split in steps 1, 4 and 7, to the extent
described in section 2.

---

## 8. Out of scope

Named explicitly so they are not built speculatively:

- **Frame pacing changes of any kind.** The algorithm is carried over unchanged by decision
  6.6: timer-clocked fixed step, single buffer, CPU upscale on every paint. Specifically out
  of scope are vsync- or refresh-driven wakes, matching the tick rate to
  `QScreen::refreshRate()`, moving shading from the timer clock to paint time, and caching the
  upscaled image. This redesign makes the current pacing observable; it does not improve it,
  and no step in section 7 should be judged on whether the output looks smoother.
- **Threaded rendering.** Would require reinstating double buffering inside `FireRenderer`.
- **Frame interpolation.** See decisions 6.4 and 6.6.
- **Post-process effects** (glow, bloom, scanlines). See decision 6.1.
- **A general frame graph** with declared resources and dependency sorting.
- **A GPU path.** Worth knowing that `PixelBuffer` and `FirePalette` map onto a texture
  upload and a LUT sampler if `QOpenGLWidget` is ever wanted, but nothing here should be
  shaped around that today.

---

## 9. Open questions

- Whether `StatsPanel` should also display counters — total ticks, frames that discarded
  time, frames skipped for zero ticks. `FrameMetricsCollector` would gain a private counter
  per displayed value and expose it through `FrameMetricsSnapshot`. None is specified until
  one has a consumer.
- (Decided, 4Hz) What `StatsPanel`'s refresh rate should be. Roughly 4 Hz is assumed in sections 3 and 4.5.
  The underlying numbers are rolling statistics, so a faster refresh trades legibility for no
  additional accuracy.
- Whether `Viewport` earns its own translation unit or should stay a private helper on
  `FireView`. It is extracted here for testability; the arithmetic is small.
- Whether `FrameReport::frameIndex` should count wakes rather than produced frames. It counts
  produced frames, so `frameIndex` and the wake count diverge by exactly the number of
  zero-tick wakes — which is arguably the more useful pair to display, and arguably confusing
  in a field named "frame".
