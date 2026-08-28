# Review resolution ownership

> I want you to review a specific part of the implementation - how width/height parameters are handled.
> The design decision is to separate simulation resolution from presentation resolution. Simulation in pure c++, Qt handles raterization. What I want you to focus on is who owns what sizes.
> Currenly, it feels a bit chaotic - no well-defined source of truth, duplicate size boundaries checks, etc. Run the analysis and suggest areas to improve.

<details><summary>7 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll trace size ownership from the simulation boundary through Qt rasterization, including constructors, resize paths, validation, and tests. I’ll keep this to an evidence-backed review and recommendations—no code changes.
>
> <details><summary>Explored a file, a search, ran a command</summary>
>
> <details><summary>Explored a file, a search</summary>
>
> - Read `./README.md`
> - Searched for `\b(width|height|resolution|size|resize|dimensions|extent|viewport|raster)\b` in `.`
>
> </details>
>
> - Ran `rg --files -g '!build/**' -g '!cmake-build-*/**' | sort && git status --short && sed -n '1,220p' src/sim/FireSimulation.hpp && sed -n '1,260p' src/sim/FireSimulation.cpp && sed -n '1,180p' src/sim/HeatFrame.hpp && sed -n '1,180p' src/engine/PixelBuffer.hpp && sed -n '1,180p' src/engine/PixelBuffer.cpp && sed -n '1,180p' src/engine/FireRenderer.hpp && sed -n '1,220p' src/engine/FireRenderer.cpp`
>
> </details>
>
> The current pipeline already separates widget size from frame size, but there are several competing representations of the frame geometry. I’m now checking which layer actually chooses it, whether that choice can change, and where type conversions or lifecycle assumptions weaken the boundary.
>
> <details><summary>Explored 21 files, a search, ran 4 commands</summary>
>
> <details><summary>Explored 21 files, a search</summary>
>
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/engine/FireEngine.cpp`
> - Read `./src/engine/FrameClock.hpp`
> - Read `./src/engine/FrameClock.cpp`
> - Read `./src/app/FireController.hpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/app/MainWindow.hpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./src/main.cpp`
> - Read `./src/app/Utils.hpp`
> - Read `./tests/engine/RenderTests.cpp`
> - Read `./tests/engine/FrameClockTests.cpp`
> - Read `./tests/app/FireViewTests.cpp`
> - Read `./tests/sim/FireSimulationTests.cpp`
> - Read `./tests/app/FireControllerTests.cpp`
> - Read `./src/sim/CMakeLists.txt`
> - Read `./src/engine/CMakeLists.txt`
> - Read `./src/app/CMakeLists.txt`
> - Read `./CMakeLists.txt`
> - Searched for `FireSimulation\{|FireEngine\{|PixelBuffer|HeatFrame|rewrap|fitPreservingAspect|SIMULATION_(WIDTH|HEIGHT)|resize\(` in `src`
>
> </details>
>
> - Ran `git log --oneline --decorate -15 && git log --oneline --all -- src/sim/HeatFrame.hpp src/engine/PixelBuffer.hpp src/engine/FireRenderer.cpp src/app/FireView.cpp src/engine/FireEngine.hpp && git diff -- README.md`
>
> - Ran `git blame -L 10,40 src/engine/FireEngine.hpp && git blame -L 1,90 src/app/FireView.cpp && git blame -L 1,50 src/engine/FireRenderer.cpp && cmake --build --preset build-release`
>
> - Ran `ctest --preset test-release --output-on-failure`
>
> - Ran `sed -n '1,260p' CMakePresets.json && ctest --test-dir build/release --output-on-failure`
>
> </details>
>
> The geometry-focused tests pass so far, but the full release suite has an unrelated engine-clock failure: the production tick rate is 90 Hz while its test still assumes 60 Hz. I’m letting the remaining tests finish; this does not change the size-ownership findings.
>
> <details><summary>Ran 4 commands</summary>
>
> - Ran `git show --stat --oneline b4a070f && git show --format=fuller --find-renames b4a070f -- src/engine/FireEngine.hpp src/app/FireController.hpp src/app/MainWindow.cpp && nl -ba src/engine/FireEngine.hpp | sed -n '1,90p' && nl -ba src/engine/FireEngine.cpp | sed -n '1,120p' && nl -ba src/engine/FireRenderer.cpp | sed -n '1,90p' && nl -ba src/engine/PixelBuffer.cpp | sed -n '1,80p' && nl -ba src/app/FireView.cpp | sed -n '1,110p' && nl -ba src/app/MainWindow.cpp | sed -n '1,110p' && nl -ba src/sim/HeatFrame.hpp | sed -n '1,90p' && nl -ba src/sim/FireSimulation.cpp | sed -n '45,90p'`
> - Ran `git show b4a070f^:docs/render_pipeline_redesign.md | rg -n -C 4 "resolution|width|height|size|resize|ownership|PixelBuffer|HeatFrame|FireView|source of truth"`
> - Ran `git show --format=fuller --stat 3ad7486 && git show --format= --find-renames 3ad7486 -- src/app/FireView.cpp tests/app/FireViewTests.cpp src/engine/FireRenderer.cpp && git show --format=fuller --stat 0a0a0ea && git show --format= 0a0a0ea -- src/app/FireView.cpp src/app/FireView.hpp`
> - Ran `nl -ba src/app/FireView.hpp | sed -n '1,90p' && nl -ba src/engine/PixelBuffer.hpp | sed -n '1,80p' && nl -ba src/engine/FireRenderer.hpp | sed -n '1,70p' && nl -ba src/sim/FireSimulation.hpp | sed -n '1,100p' && nl -ba tests/engine/RenderTests.cpp | sed -n '118,160p' && nl -ba tests/app/FireViewTests.cpp | sed -n '35,85p' && nl -ba tests/app/StatsPanelTests.cpp | sed -n '126,152p' && git status --short`
>
> </details>
>
> The key issue is now clear: the code simultaneously promises that render geometry may change and assumes that it never changes. The renderer/test suite supports reallocating the pixel buffer, while `FireView` permanently aliases the buffer created at startup. That contradiction is the main source-of-truth and ownership risk; the repeated scalar fields are mostly a symptom.
</details>

The simulation/presentation split is structurally sound: pure C++ produces a fixed-resolution RGBA frame, while Qt scales it into the widget. The weakness is that the code has not committed to whether frame geometry is immutable or dynamically resizable.

### Current ownership

| Size | Current authority/owner | Assessment |
|---|---|---|
| Simulation resolution | `FireEngine` defaults or constructor caller | Effective configuration source, but represented as two unrelated scalars |
| Simulation storage geometry | `FireSimulation` | Good: logical size is separated from internal padded width/stride |
| Heat-frame geometry | `HeatFrame` view | Correctly travels with the data, but is not an independent authority |
| RGBA frame geometry/storage | `FireRenderer` through `PixelBuffer` | Automatically follows every `HeatFrame` |
| Presentation source image | `FireView`’s `QImage` | Wraps the pixel storage once and assumes it never moves |
| Presentation resolution | `QWidget`, layouts, and `MainWindow` | Correct Qt ownership, although size hints partly couple it to simulation resolution |

### Findings

1. High: the renderer and view have contradictory resize contracts.

[FireRenderer.cpp](./src/engine/FireRenderer.cpp:12) reallocates its target whenever the incoming geometry changes. The renderer test explicitly promises this behavior in [RenderTests.cpp](./tests/engine/RenderTests.cpp:146).

Meanwhile, [FireView.cpp](./src/app/FireView.cpp:16) wraps the buffer only during construction. `present()` merely schedules painting, and the `QImage` continues pointing at the original allocation. A renderer resize can therefore leave it holding a dangling pointer and stale dimensions.

The application is currently safe only because `FireSimulation` cannot resize after construction. That invariant is real, but it is not expressed by the renderer or view interfaces.

2. High: allocation-capable resizing exists below `noexcept` steady-state code.

`FireEngine::advance()`, `reset()`, palette changes, and `shade()` are `noexcept`, but eventually call `FireRenderer::render()`, which may call the throwing [PixelBuffer::resize()](./src/engine/PixelBuffer.cpp:7).

With unchanged geometry it does not allocate, but the type system does not encode that. A geometry mismatch during runtime would terminate the process rather than produce an initialization error. This conflicts with the repository rule that exceptions and allocation-related failures belong in initialization.

3. Medium: there is no named, immutable simulation-resolution value.

The initial choice currently resides in two private constants/default arguments in [FireEngine.hpp](./src/engine/FireEngine.hpp:15). It is then repeatedly represented as `width` and `height` in the simulation, `HeatFrame`, `PixelBuffer`, and Qt conversion.

Those buffers legitimately need shape metadata; that duplication is not itself wrong. The problem is the absence of a shared concept such as `Extent2D` or validated `SimulationExtent`, leaving every interface to accept two positional integers and define its own validity rules.

4. Medium: validation responsibilities are fragmented.

There are four distinct checks:

- `FireSimulation`: algorithm minimum and allocation arithmetic.
- `HeatFrame`: shape agreement, but only through `assert` in [HeatFrame.hpp](./src/sim/HeatFrame.hpp:11).
- `PixelBuffer`: area overflow.
- `FireView`: Qt `int` and scan-line representability in [FireView.cpp](./src/app/FireView.cpp:30).

The simulation-specific and Qt-specific checks should remain at their respective boundaries. The generic area arithmetic and empty-geometry semantics could be centralized. Currently, for example, buffers permit `0 × N`, the simulation requires at least `2 × 2`, and an unrepresentable Qt image silently becomes empty.

5. Medium: presentation policy is partially derived from simulation resolution.

Qt correctly owns the actual presentation size, and [paintEvent()](./src/app/FireView.cpp:61) scales the simulation frame into that area.

However, `sizeHint()` equals the simulation frame size and `minimumSizeHint()` equals half of it. This means changing simulation fidelity also changes layout preferences and minimum window constraints. If the resolutions are meant to be independent, minimum and preferred presentation sizes should be explicit UI policy—or the view should let the layout ignore its natural image size. The source aspect ratio should still come from the frame.

6. Low: `FireView` keeps redundant geometry state.

`wrappedData`, `wrappedWidth`, and `wrappedHeight` in [FireView.hpp](./src/app/FireView.hpp:34) are only used to construct the `QImage`. Afterwards, the `QImage` already stores the pointer and source dimensions. These members make it look as though the view actively tracks the buffer when it does not.

### Recommended ownership model

I would formalize the existing fixed-resolution behavior:

- The composition configuration selects one validated `SimulationExtent`.
- `FireEngine` owns that immutable extent, the simulation, and the long-lived rendered frame.
- `FireSimulation` owns only simulation state and its private padded storage geometry.
- `HeatFrame` carries the extent with a non-owning view; it communicates shape but does not choose it.
- `FireRenderer` owns shading policy, not geometry policy. Its steady-state operation requires matching input/output extents and is `noexcept`.
- `FireView` owns presentation size and aspect-fit policy. It either:
  - owns/copies its `QImage`, giving the cleanest lifetime boundary; or
  - borrows a rendered frame whose allocation is explicitly guaranteed stable for the view’s lifetime.

For the current zero-copy implementation, the second option is probably the smallest coherent design: construct the rendered buffer once from the validated extent, remove runtime resizing, and make resolution changes an explicit initialization/reconfiguration operation that rebuilds the whole frame/view relationship.

Tests should then stop promising arbitrary renderer resizing and instead verify:

- configured extent propagates unchanged through simulation, heat frame, and rendered frame;
- frame storage never moves across ticks, reset, or palette changes;
- widget resizing never changes simulation/frame geometry;
- invalid simulation geometry is rejected once during initialization;
- Qt representability is handled explicitly at the Qt boundary.

Build completed successfully. Eight of nine release tests pass; the only failure is unrelated to geometry—the engine clock test still assumes 60 Hz while production now uses 90 Hz. No files were changed.

::code-comment{title="Conflicting resize contract" body="The renderer reallocates its target when frame geometry changes, but FireView permanently aliases the original allocation. Either make target geometry immutable after initialization or publish geometry/storage changes so the view can safely rewrap." file="/Users/aleksandr.voloshanenko/private/fire/src/engine/FireRenderer.cpp" start=13 end=14 priority=1}

::code-comment{title="Simulation-derived presentation minimum" body="Deriving the widget minimum from simulation pixels couples simulation fidelity to presentation policy. Prefer an explicit UI minimum or a layout policy independent of the source resolution." file="/Users/aleksandr.voloshanenko/private/fire/src/app/FireView.cpp" start=49 priority=2}

> I like suggested ownership model. Use second approach for FireView - borrowed guaranteed size, configured at compile time.
>
> You can go ahead and implement this approach

<details><summary>10 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll implement the fixed-resolution contract end to end: one compile-time extent at the composition root, immutable pure-C++ frame geometry, a renderer that never reallocates during steady state, and an explicitly lifetime-safe borrowed buffer in `FireView`. I’ll also adjust the tests so they enforce that contract instead of dynamic renderer resizing.
>
> <details><summary>Created a file, edited 4 files, explored 2 files, 2 searches, ran a command</summary>
>
> - Ran `sed -n '1,180p' README.md && git status --short && rg -n "FireController\s+[a-zA-Z]|FireController\{|new FireController|FireEngine\{|FireSimulation\{|HeatFrame\{|PixelBuffer\s+[a-zA-Z]|PixelBuffer\{|\.resize\(" src tests --glob '*.{cpp,hpp,h}'`
>
> - Wrote `./src/sim/CMakeLists.txt` (+1 -0)
> - Wrote `./src/sim/Extent2D.hpp` (+23 -0)
> - Wrote `./src/sim/FireSimulation.cpp` (+20 -18)
> - Wrote `./src/sim/FireSimulation.hpp` (+6 -5)
> - Wrote `./src/sim/HeatFrame.hpp` (+12 -14)
>
> <details><summary>Explored 4 files, 2 searches</summary>
>
> - Searched for `simulationWidth|simulationHeight|HeatFrame\{` in `sim`
> - Read `./src/sim/FireSimulation.cpp`
> - Read `./src/sim/Extent2D.hpp`
> - Read `./src/sim/FireSimulation.cpp`
> - Read `./src/sim/Extent2D.hpp`
> - Searched for `simulationWidth|simulationHeight|HeatFrame\{` in `sim`
>
> </details>
>
> </details>
>
> The pure-C++ geometry is now represented by one `Extent2D` value, and the simulation keeps only its private padded dimensions separately. Next I’m removing render-target resizing entirely so buffer allocation can occur only during engine construction.
>
> <details><summary>Edited 17 files, explored 2 searches, ran 5 commands</summary>
>
> - Wrote `./src/engine/FireEngine.cpp` (+8 -5)
> - Wrote `./src/engine/FireEngine.hpp` (+7 -5)
> - Wrote `./src/engine/FireRenderer.cpp` (+6 -4)
> - Wrote `./src/engine/FireRenderer.hpp` (+1 -4)
> - Wrote `./src/engine/PixelBuffer.cpp` (+8 -7)
> - Wrote `./src/engine/PixelBuffer.hpp` (+8 -5)
>
> - Wrote `./src/app/FireController.cpp` (+2 -1)
> - Wrote `./src/app/FireController.hpp` (+2 -1)
> - Wrote `./src/app/FireView.cpp` (+30 -30)
> - Wrote `./src/app/FireView.hpp` (+1 -10)
> - Wrote `./src/app/MainWindow.cpp` (+19 -3)
> - Wrote `./src/app/MainWindow.hpp` (+3 -0)
>
> <details><summary>Explored 2 searches</summary>
>
> - Searched for `FireSimulation\{|FireEngine\{|FireController\s+[a-zA-Z]|HeatFrame\{|\.render\(|\.resize\(` in `src`
> - Searched for `SIMULATION_WIDTH|UNALIGNED_WIDTH|FireSimulation |FireEngine engine|HeatFrame` in `tests`
>
> </details>
>
> - Wrote `./src/engine/PixelBuffer.hpp` (+4 -0)
> - Wrote `./tests/app/FireControllerTests.cpp` (+6 -4)
> - Wrote `./tests/app/FireViewTests.cpp` (+8 -12)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+4 -4)
> - Wrote `./tests/engine/RenderTests.cpp` (+17 -21)
>
> - Wrote `./tests/sim/FireSimulationTests.cpp` (+12 -11)
>
> - Ran `rg -n "FireSimulation\s+[A-Za-z_].*\{[^\{]*,[^\{]*,|FireEngine engine\{[0-9]|FireController controller;|HeatFrame\{[^\n]*, [0-9]|buffer\.resize|renderer\.target|renderer\.render\([^,\n]*\)" src tests --glob '*.{cpp,hpp,h}'
> rg -n "FireEngine\(|FireController\(|FireSimulation\(|HeatFrame\(" src tests --glob '*.{cpp,hpp,h}'
> git diff --stat
> git diff --check`
>
> - Ran `cmake --build --preset build-debug`
>
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> - Ran `rg -n "TICK_DURATION|PRODUCTION_TICK" src/engine/FireEngine.hpp tests/engine/FrameClockTests.cpp
> build/debug/src/engine/fire_engine_clock_tests`
>
> - Ran `git diff -- tests/engine/FrameClockTests.cpp
> git log -1 --oneline
> git status --short`
>
> </details>
>
> A concurrent repository update landed while I was working: `HEAD` advanced from `4b83733` to `445192c`, including the corrected 90 Hz clock tests. My geometry changes remain cleanly layered on top and the debug suite now passes fully. I’m checking the new base diff for overlap before final verification.
>
> <details><summary>Edited 7 files, ran 4 commands</summary>
>
> - Ran `git log --oneline -4
> git show --stat --oneline 445192c
> git diff 4b83733..445192c -- src tests`
> - Ran `rg -n "PixelBuffer\s+[A-Za-z_][A-Za-z0-9_]*(\s*[;{])|PixelBuffer\{" src tests --glob '*.{cpp,hpp,h}'
> git diff -- src/engine src/sim src/app tests/engine/RenderTests.cpp tests/app/FireViewTests.cpp tests/app/FireControllerTests.cpp tests/sim/FireSimulationTests.cpp`
> - Wrote `./src/app/MainWindow.cpp` (+8 -10)
> - Wrote `./src/app/MainWindow.hpp` (+3 -1)
> - Wrote `./src/engine/PixelBuffer.cpp` (+2 -3)
> - Wrote `./src/engine/PixelBuffer.hpp` (+0 -1)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+2 -1)
> - Wrote `./tests/engine/RenderTests.cpp` (+1 -1)
> - Wrote `./tests/sim/FireSimulationTests.cpp` (+2 -0)
> - Ran `cmake --build --preset build-debug`
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> </details>
>
> The fixed-resolution path now builds and all nine debug tests pass. The ownership guarantee is encoded rather than documented only: `PixelBuffer` cannot resize, copy, or move; `FireView` stores an explicit borrow; and `MainWindow` owns the view with RAII so the borrower is destroyed before the controller’s frame storage, including constructor-failure paths. I’m running sanitizer and release verification next.
>
> <details><summary>Ran 5 commands</summary>
>
> - Ran `cmake --build --preset build-sanitize`
> - Ran `ctest --test-dir build/sanitize --output-on-failure`
> - Ran `cmake --build --preset build-release`
> - Ran `ctest --test-dir build/release --output-on-failure`
> - Ran `git diff --check
> rg -n "resize\(|simulationWidth|simulationHeight|wrappedWidth|wrappedHeight|wrappedData|renderer\.target" src tests --glob '*.{cpp,hpp,h}'
> git status --short
> clang-format --version`
>
> </details>
>
> Debug, release, and ASan/UBSan suites all pass. The remaining `resize()` calls are now only initialization-time vector allocation or Qt widget presentation resizing—there is no runtime frame-geometry mutation left. I’m applying the required repository formatting to only the C++ files touched by this change.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `clang-format -i src/app/FireController.cpp src/app/FireController.hpp src/app/FireView.cpp src/app/FireView.hpp src/app/MainWindow.cpp src/app/MainWindow.hpp src/engine/FireEngine.cpp src/engine/FireEngine.hpp src/engine/FireRenderer.cpp src/engine/FireRenderer.hpp src/engine/PixelBuffer.cpp src/engine/PixelBuffer.hpp src/sim/Extent2D.hpp src/sim/FireSimulation.cpp src/sim/FireSimulation.hpp src/sim/HeatFrame.hpp tests/app/FireControllerTests.cpp tests/app/FireViewTests.cpp tests/engine/FrameClockTests.cpp tests/engine/RenderTests.cpp tests/sim/FireSimulationTests.cpp`
> - Ran `git diff --check
> git diff --stat
> git status --short
> nl -ba src/sim/Extent2D.hpp
> nl -ba src/engine/FireEngine.hpp | sed -n '12,55p'
> nl -ba src/engine/PixelBuffer.hpp | sed -n '8,40p'
> nl -ba src/engine/FireRenderer.cpp | sed -n '8,40p'
> nl -ba src/app/FireView.cpp | sed -n '10,70p'
> nl -ba src/app/MainWindow.cpp | sed -n '15,110p'`
>
> </details>
</details>

Implemented the fixed-resolution ownership model.

Key changes:

- Added pure-C++ [`Extent2D`](./src/sim/Extent2D.hpp:7).
- Simulation resolution is configured at compile time in [`MainWindow.cpp`](./src/app/MainWindow.cpp:20).
- [`FireEngine`](./src/engine/FireEngine.hpp:40) owns the immutable extent and rendered frame.
- `PixelBuffer` is allocated once and cannot resize, copy, or move.
- [`FireRenderer::render()`](./src/engine/FireRenderer.cpp:13) is now allocation-free and `noexcept`.
- [`FireView`](./src/app/FireView.cpp:38) explicitly borrows stable pixel storage.
- `MainWindow` uses RAII to destroy the borrowing view before the controller and frame storage.
- Presentation dimensions are independent from simulation dimensions through `QSizePolicy::Ignored`.
- Tests now enforce fixed geometry and stable storage rather than renderer-driven resizing.

Verification:

- Debug: 9/9 tests passed
- Release: 9/9 tests passed
- ASan + UBSan: 9/9 tests passed
- `clang-format` applied to all modified C++ files
- `git diff --check` clean

A concurrent `HEAD` update containing the corrected 90 Hz clock tests was incorporated cleanly. The pre-existing [`README.md`](./README.md) modification was preserved and not edited.

> What was the motivation for using unique\_ptr\<FireView> in MainWindow?

`FireView`’s `QImage` directly aliases the `PixelBuffer` owned by `FireController`. Therefore the view must be destroyed before the controller.

`std::unique_ptr<FireView>` makes that ordering explicit:

- `MainWindow` members are destroyed before its `QObject` base destroys child objects.
- The `unique_ptr` deletes `FireView` first.
- `FireController`, still owned as a QObject child, remains alive until afterward.
- It also preserves the ordering if `MainWindow` construction throws after creating the view.

Although `FireView` also has a Qt parent, this is safe: deleting it through the `unique_ptr` removes it from the parent’s child list, preventing double deletion.

The alternatives were weaker:

- Relying on Qt child insertion order would leave an implicit lifetime dependency.
- Manually deleting a raw pointer in `MainWindow::~MainWindow()` would not cover constructor-failure cleanup.
- Copying pixels into a view-owned `QImage` would remove the lifetime dependency but contradict the chosen zero-copy approach.

So the `unique_ptr` primarily encodes the borrowed-buffer lifetime contract, not ordinary widget ownership. It is slightly unconventional in Qt, but deliberate here.

> I don;t like Extent2D name. Does Dimensions name fits here instead?

Yes, `Dimensions` fits better than `Extent2D`: the type simply represents a width/height pair, not a coordinate-space extent.

I’d slightly prefer `FrameDimensions`, because plain `Dimensions` is broad and lives in the global namespace. It accurately applies to:

- simulation dimensions
- `HeatFrame` dimensions
- `PixelBuffer` dimensions

It also avoids confusion with Qt’s `QSize`.

My recommendation: rename `Extent2D` to `FrameDimensions`. If you prefer the shorter name and don’t anticipate other dimension types, `Dimensions` is still perfectly reasonable.

> Is there really any point in defininig extent in MainWindow and then forwarding it to FireEngine through FireController?

Not really, given the decision that simulation dimensions are fixed at compile time.

Keeping them in `MainWindow` only makes sense if the application selects among configurations. Currently it forces the Qt composition code to know about simulation geometry and turns `FireController` into a forwarding layer.

A cleaner arrangement would be:

```cpp
class FireEngine final {
public:
    FireEngine();
    explicit FireEngine(FrameDimensions dimensions);

private:
    static constexpr FrameDimensions DEFAULT_DIMENSIONS{600, 400};
};
```

Then:

- Production `FireController` default-constructs `FireEngine`.
- `MainWindow` knows nothing about simulation dimensions.
- Tests can still construct `FireEngine` with small custom dimensions.
- The engine’s immutable dimensions remain the runtime source of truth.

If controller tests need smaller dimensions for speed, `FireController` can retain an explicit dimensions overload for tests and specialized initialization, while its normal constructor uses the engine default.

So yes: I would move the compile-time default into `FireEngine` and remove the production forwarding through `MainWindow` and `FireController`.

> Things to do:
>
> - Rename Extend2D -> Dimensions
> - Move compile-time defaults to FireEngine
>
> Things to consider:
>
> - Check the attached review and append things to do list with reasonable requests.
>
> Do not implement it. As a result I expect a refactoring TODO list: bullet points and motivation brief. Also, at the end provide the list of dismissed comments and brief motivation.
>
> User context
>
> Attachments:
> - What it gets right — and these are substantial: 1. The renderer became a pure f…: `~/.codex/attachments/8385134d-db6f-4d86-ac9d-1fd8695e576b/pasted-text.txt`

<details><summary>2 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll compare the attached review against the current implementation and turn only the well-supported points into a prioritized refactoring TODO list. I’ll keep implementation out of scope and explicitly separate dismissed suggestions with reasons.
>
> <details><summary>Explored 11 files</summary>
>
> - Read `~/.codex/attachments/8385134d-db6f-4d86-ac9d-1fd8695e576b/pasted-text.txt`
> - Read `~/.codex/attachments/8385134d-db6f-4d86-ac9d-1fd8695e576b/pasted-text.txt`
> - Read `./src/app/StatsPanel.hpp`
> - Read `./src/app/MainWindow.hpp`
> - Read `./src/app/MainWindow.cpp`
> - Read `./src/engine/PixelBuffer.hpp`
> - Read `./src/engine/PixelBuffer.cpp`
> - Read `./src/app/FireView.hpp`
> - Read `./src/app/FireView.cpp`
> - Read `./src/engine/FireRenderer.cpp`
> - Read `./src/engine/FireEngine.hpp`
>
> </details>
</details>

## Refactoring TODO

- Rename `Extent2D` to `Dimensions`.

  Rename the file, type, accessors, members, and constants consistently: `extent()` → `dimensions()`, `simulationExtent` → `simulationDimensions`, and so on. `Dimensions` better describes a width/height value and avoids coordinate-space connotations.

- Move the compile-time default dimensions into `FireEngine`.

  Add a default/delegating `FireEngine` constructor using `DEFAULT_DIMENSIONS`, while retaining the explicit `Dimensions` constructor for tests. Remove dimension forwarding from `MainWindow` and the normal `FireController` construction path. The Qt composition layer has no reason to know the fixed simulation resolution.

- Relocate `Dimensions` out of `src/sim`.

  It is shared vocabulary for simulation frames, rendered buffers, and Qt presentation input. Put it in a neutral pure-C++ location such as `src/core/Dimensions.hpp` or `src/common/Dimensions.hpp`, rather than implying simulation ownership.

- Resolve the dual ownership of `FireView`.

  `FireView` currently has both a Qt parent and a `std::unique_ptr` owner. Qt can delete the view when its parent hierarchy changes, leaving the `unique_ptr` dangling. Establish one authoritative owner—preferably Qt ownership—and separately encode the requirement that frame storage outlive the borrowing view.

- Document the reason `PixelBuffer` cannot move or resize.

  Place the zero-copy lifetime rationale beside the deleted copy/move operations. Their purpose is to keep the address wrapped by `QImage` stable, which is not currently evident from `PixelBuffer.hpp`.

- Remove redundant `FireView` state and checks.

  Remove the per-frame pointer/dimension assertions and, if no longer needed, the `borrowedPixels` reference. Those assertions cannot safely detect a dangling reference and otherwise recheck guarantees already provided by immutable `PixelBuffer` storage. Remove the empty-buffer check because `PixelBuffer` construction already rejects empty dimensions.

- Define one renderer dimension-mismatch policy.

  `FireRenderer` currently asserts in debug but silently preserves a stale frame in release. Treat a mismatch explicitly as either:

  - a programming contract violation with a clearly documented invariant/failure path; or
  - a recoverable error represented by a non-throwing result and covered by tests.

  Avoid leaving debug and release with materially different undocumented behavior.

- Remove the redundant dimensions copy from `FireEngine`.

  `FireEngine`, `FireSimulation`, and `PixelBuffer` currently store the same value. Let the simulation or rendered frame retain the authoritative instance and have `FireEngine::dimensions()` forward to it.

- Clarify validation ownership.

  Keep `Dimensions` as a vocabulary value, while assigning validation deliberately:

  - `FireEngine` statically validates its compile-time default.
  - `FireSimulation` validates algorithm requirements such as the `2 × 2` minimum.
  - `PixelBuffer` validates non-empty, representable allocation geometry during initialization.
  - `FireView` validates the Qt-specific `QImage` limits.

  Consider preventing accidental default construction if `Dimensions{0, 0}` is never a valid frame configuration.

- Validate derived simulation storage dimensions directly.

  Perform checked arithmetic after lattice-width rounding and while adding source rows. Checks should describe `fieldWidth`, `fieldStride`, cooling-map area, and heat-field area—the actual allocation dimensions—rather than approximating them from the requested width.

- Prune unused scalar accessors after the rename.

  Remove `width()` or `height()` methods that have no production users, while retaining derived convenience accessors where they make row-processing code clearer. All retained accessors must derive from `Dimensions`, never duplicate storage.

- Give the aspect-fit helper a focused home.

  Move `fitPreservingAspect` from generic `app/Utils.hpp` into a named header such as `AspectFit.hpp` or `Viewport.hpp`, and place its tests alongside the presentation/view tests rather than `StatsPanelTests`.

## Dismissed comments

- Use `std::shared_ptr<const PixelBuffer>` to fix `FireView` lifetime.

  Shared ownership is unnecessary unless the view must independently outlive the rendering session. It would introduce heap/shared-ownership semantics into the engine frame path. Fix the dual ownership and composition lifetime directly first.

- Apply the same shared-ownership fix to `StatsPanel`.

  `StatsPanel` is not simultaneously owned by Qt and a smart pointer. Replacing the central widget destroys the panel before its referenced collector, so it does not have the demonstrated double-deletion defect. Its borrow can be documented without introducing shared ownership.

- Make `Dimensions` universally validated and replace all runtime checks with `static_assert`.

  Simulation, storage, and Qt have different constraints. Additionally, explicit dimensions remain useful for tests and may be supplied at runtime during initialization. Only the production default can be completely compile-time validated.

- Move the Qt `QImage` ceiling into `FireEngine`.

  That would make the pure-C++ engine aware of a Qt presentation constraint and weaken the simulation/Qt boundary. The check belongs in `FireView`.

- Remove all `const` dimension members.

  Fixed dimensions are intentional, and immovability is load-bearing for the zero-copy `QImage`. Remove only genuinely redundant copies, such as the extra engine member, rather than weakening the invariant everywhere.

- Restore `PixelBuffer` movability for potential `optional` or factory use.

  Moving the buffer would invalidate the borrowed `QImage`. Hypothetical container convenience does not outweigh the current address-stability requirement.

- Remove all `width()` and `height()` accessors categorically.

  Derived accessors do not create another source of truth. Some remain useful for loops and row-oriented APIs; only unused or duplicative surface area should be removed.

- Drop the renderer’s release mismatch guard without replacement.

  An assertion disappears in release builds. Removing the guard outright could turn a contract violation into out-of-bounds access. First define the failure policy, then implement and test it accordingly.

> Ok, do the refactoring step by step according to TODO list

<details><summary>12 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll apply the accepted TODOs in dependency order: establish the shared `Dimensions` vocabulary and engine defaults first, then fix ownership/lifetime, simplify contracts and validation, and finish with API/test cleanup. I’ll preserve the existing `README.md` change and the concurrent simulation update.
>
> <details><summary>Explored 4 files, ran a command</summary>
>
> - Ran `sed -n '1,240p' cmake/FireTargets.cmake
> sed -n '1,120p' src/sim/CMakeLists.txt
> sed -n '1,120p' src/engine/CMakeLists.txt
> sed -n '1,180p' tests/tests_common.h
> git status --short
> rg -n "Extent2D|extent\(\)|simulationExtent|bufferExtent|fitPreservingAspect|Utils.hpp|FireController\{" src tests --glob '*.{cpp,hpp,h}'`
>
> <details><summary>Explored 4 files</summary>
>
> - Read `./cmake/ProjectSettings.cmake`
> - Read `./src/metrics/CMakeLists.txt`
> - Read `./CMakeLists.txt`
> - Read `./src/app/CMakeLists.txt`
>
> </details>
>
> </details>
>
> The first design decision is settled: `Dimensions` will be a neutral, non-default-constructible vocabulary type in a small header-only core target. `FireEngine` will own the production default and keep an explicit dimensions constructor for tests; `MainWindow` and its normal controller path will stop forwarding resolution.
>
> <details><summary>Created 2 files, edited 13 files, deleted a file</summary>
>
> - Wrote `./CMakeLists.txt` (+1 -0)
> - Wrote `./src/core/CMakeLists.txt` (+4 -0)
> - Wrote `./src/core/Dimensions.hpp` (+25 -0)
> - Wrote `./src/engine/FireEngine.hpp` (+5 -4)
> - Wrote `./src/engine/PixelBuffer.hpp` (+4 -6)
> - Wrote `./src/sim/CMakeLists.txt` (+2 -1)
> - Deleted `./src/sim/Extent2D.hpp` (+0 -23)
> - Wrote `./src/sim/FireSimulation.hpp` (+4 -6)
> - Wrote `./src/sim/HeatFrame.hpp` (+11 -11)
> - Wrote `./src/app/FireController.cpp` (+7 -2)
> - Wrote `./src/app/FireController.hpp` (+3 -2)
> - Wrote `./src/app/MainWindow.cpp` (+1 -6)
> - Wrote `./src/engine/FireEngine.cpp` (+4 -3)
> - Wrote `./src/engine/FireRenderer.cpp` (+2 -2)
> - Wrote `./src/engine/PixelBuffer.cpp` (+6 -6)
> - Wrote `./src/sim/FireSimulation.cpp` (+20 -18)
>
> </details>
>
> The production dimension forwarding is removed: `FireEngine` now supplies its own compile-time default, while the explicit constructor remains for fast tests. I’m now fixing the more important lifetime issue by making `FireView` exclusively Qt-owned and placing `FireController` outside `MainWindow`, so its frame storage necessarily outlives the entire window and every child view.
>
> <details><summary>Created a file, edited 19 files, deleted a file, explored 11 files, 5 searches</summary>
>
> - Wrote `./src/app/MainWindow.cpp` (+21 -26)
> - Wrote `./src/app/MainWindow.hpp` (+2 -7)
> - Wrote `./src/main.cpp` (+5 -1)
>
> <details><summary>Explored 3 files, a search</summary>
>
> - Searched for `fireController->|fireView\.get|unique_ptr<FireView>|Extent2D|\.extent\(\)|simulationExtent|bufferExtent|frameExtent` in `src`
> - Read `./src/app/MainWindow.cpp`
> - Read `./src/app/FireController.cpp`
> - Read `./src/engine/FireEngine.hpp`
>
> </details>
>
> - Wrote `./src/app/FireController.cpp` (+5 -3)
> - Wrote `./src/app/FireController.hpp` (+1 -0)
> - Wrote `./src/app/FireView.cpp` (+9 -18)
> - Wrote `./src/app/FireView.hpp` (+0 -1)
> - Wrote `./src/engine/FireEngine.hpp` (+3 -0)
> - Wrote `./src/engine/FireRenderer.cpp` (+4 -3)
> - Wrote `./src/engine/PixelBuffer.hpp` (+3 -0)
>
> <details><summary>Explored 2 files, a search</summary>
>
> - Read `./src/sim/FireSimulation.cpp`
> - Read `./src/engine/PixelBuffer.cpp`
> - Searched for `std::ptrdiff_t|MAXIMUM_SIZE|roundUp` in `FireSimulation.cpp`
>
> </details>
>
> - Wrote `./src/engine/PixelBuffer.cpp` (+1 -1)
> - Wrote `./src/sim/FireSimulation.cpp` (+17 -5)
>
> <details><summary>Explored 3 files</summary>
>
> - Read `./tests/app/StatsPanelTests.cpp`
> - Read `./tests/app/FireViewTests.cpp`
> - Read `./src/app/Utils.hpp`
>
> </details>
>
> - Wrote `./src/app/AspectFit.hpp` (+32 -0)
> - Wrote `./src/app/CMakeLists.txt` (+1 -1)
> - Wrote `./src/app/FireView.cpp` (+1 -1)
> - Deleted `./src/app/Utils.hpp` (+0 -32)
> - Wrote `./tests/app/FireViewTests.cpp` (+27 -1)
> - Wrote `./tests/app/StatsPanelTests.cpp` (+0 -23)
>
> - Wrote `./tests/app/FireControllerTests.cpp` (+5 -5)
> - Wrote `./tests/engine/FrameClockTests.cpp` (+2 -2)
> - Wrote `./tests/engine/RenderTests.cpp` (+2 -2)
> - Wrote `./tests/sim/FireSimulationTests.cpp` (+12 -14)
>
> <details><summary>Explored 3 searches</summary>
>
> - Searched for `Extent2D|EXTENT|\.extent\(\)|simulationExtent|bufferExtent|frameExtent|simulation\.width|simulation\.height|buffer\.width|buffer\.height|Utils.hpp|unique_ptr<FireView>` in `src`
> - Searched for `SIMULATION_DIMENSIONS|FireSimulation .*SIMULATION` in `FireSimulationTests.cpp`
> - Searched for `Dimensions\s+[A-Z_]+\{|Dimensions\{` in `src`
>
> </details>
>
> - Wrote `./tests/engine/RenderTests.cpp` (+1 -2)
> - Wrote `./tests/sim/FireSimulationTests.cpp` (+1 -1)
>
> <details><summary>Explored 4 files</summary>
>
> - Read `./tests/sim/FireSimulationTests.cpp`
> - Read `./tests/engine/RenderTests.cpp`
> - Read `./tests/app/FireControllerTests.cpp`
> - Read `./tests/app/FireViewTests.cpp`
>
> </details>
>
> </details>
>
> The view now has exactly one owner—the Qt widget tree. The controller is created in `main()` before the window and destroyed after it, which makes the zero-copy frame lifetime correct even if the central widget is replaced. I’ve also removed the per-frame borrow assertions and moved the stability rationale to `PixelBuffer`, where the compiler-enforced invariant lives.
>
> <details><summary>Edited 5 files, explored a file, a search, ran 6 commands</summary>
>
> - Ran `cmake --build --preset build-debug`
>
> - Wrote `./tests/engine/FrameClockTests.cpp` (+1 -1)
>
> - Ran `cmake --build --preset build-debug`
>
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> - Ran `rg -n "Extent2D|EXTENT|\.extent\(\)|Utils.hpp|simulationWidth|simulationHeight|bufferWidth|bufferHeight|wrappedWidth|wrappedHeight|borrowedPixels|unique_ptr<FireView>|\.width\(\)|\.height\(\)" src tests --glob '*.{cpp,hpp,h}'
> rg -n "dimensions\(\)" src tests --glob '*.{cpp,hpp,h}'
> git diff --check
> git status --short`
>
> - Wrote `./tests/engine/FrameClockTests.cpp` (+8 -0)
> - Wrote `./tests/engine/RenderTests.cpp` (+4 -0)
>
> <details><summary>Explored 2 files, a search</summary>
>
> - Read `./tests/app/FireViewTests.cpp`
> - Read `./tests/app/FireViewTests.cpp`
> - Searched for `testEngineUses|testEngineProduces|testResetClears|int main` in `FrameClockTests.cpp`
>
> </details>
>
> - Wrote `./tests/app/FireViewTests.cpp` (+17 -0)
>
> - Ran `cmake --build --preset build-debug`
>
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> - Wrote `./src/app/MainWindow.hpp` (+1 -0)
> - Wrote `./src/core/Dimensions.hpp` (+2 -0)
>
> </details>
>
> The contract and cleanup phase is complete: mismatched renderer dimensions now fail consistently instead of silently leaving a stale frame; generic, simulation-derived, storage, and Qt validation each remain at their proper boundary; redundant engine/scalar state is gone; and the aspect-fit helper and tests now live with presentation code. I’m doing the final diff review before multi-configuration verification.
>
> <details><summary>Ran 6 commands</summary>
>
> - Ran `git diff --check
> rg -n "Extent2D|EXTENT|\.extent\(\)|Utils.hpp|unique_ptr<FireView>|borrowedPixels|simulationWidth|simulationHeight|bufferWidth|bufferHeight" src tests --glob '*.{cpp,hpp,h}'
> rg -n "Dimensions|dimensions\(\)" src tests --glob '*.{cpp,hpp,h}'
> git diff --stat
> git status --short`
> - Ran `git diff -- . ':!README.md' ':!src/sim/FireSimulation.cpp' ':!tests/sim/FireSimulationTests.cpp'
> git diff -- src/sim/FireSimulation.cpp tests/sim/FireSimulationTests.cpp`
> - Ran `cmake --build --preset build-release`
> - Ran `cmake --build --preset build-sanitize`
> - Ran `ctest --test-dir build/sanitize --output-on-failure`
> - Ran `ctest --test-dir build/release --output-on-failure`
>
> </details>
>
> All debug, release, and ASan/UBSan tests pass, including a regression test that replaces the central widget and verifies Qt can destroy `FireView` without a second owner. I’m applying the repository’s final formatting pass now; no further source edits are planned afterward.
>
> <details><summary>Ran 2 commands</summary>
>
> - Ran `clang-format -i src/app/AspectFit.hpp src/app/FireController.cpp src/app/FireController.hpp src/app/FireView.cpp src/app/FireView.hpp src/app/MainWindow.cpp src/app/MainWindow.hpp src/engine/FireEngine.cpp src/engine/FireEngine.hpp src/engine/FireRenderer.cpp src/engine/FireRenderer.hpp src/engine/PixelBuffer.cpp src/engine/PixelBuffer.hpp src/main.cpp src/sim/FireSimulation.cpp src/sim/FireSimulation.hpp src/sim/HeatFrame.hpp src/core/Dimensions.hpp tests/app/FireControllerTests.cpp tests/app/FireViewTests.cpp tests/app/StatsPanelTests.cpp tests/engine/FrameClockTests.cpp tests/engine/RenderTests.cpp tests/sim/FireSimulationTests.cpp`
> - Ran `git diff --check
> git status --short
> nl -ba src/core/Dimensions.hpp
> nl -ba src/engine/FireEngine.hpp | sed -n '14,55p'
> nl -ba src/engine/PixelBuffer.hpp | sed -n '10,40p'
> nl -ba src/engine/FireRenderer.cpp | sed -n '8,35p'
> nl -ba src/main.cpp | sed -n '1,45p'
> nl -ba src/app/MainWindow.cpp | sed -n '15,55p'
> nl -ba src/app/FireView.cpp | sed -n '12,50p'
> nl -ba src/sim/FireSimulation.cpp | sed -n '55,95p'`
>
> </details>
</details>

Implemented the full accepted refactoring TODO.

Key results:

- Replaced `Extent2D` with neutral [`Dimensions`](./src/core/Dimensions.hpp:9) in a new header-only `fire_core` layer.
- `Dimensions` cannot be default-constructed and owns only shared arithmetic—not domain validation.
- Moved `600 × 400` defaults into [`FireEngine`](./src/engine/FireEngine.hpp:35).
- Removed production dimension forwarding through `MainWindow` and `FireController`; explicit dimensions remain available for tests.
- Removed the redundant dimensions member from `FireEngine`.
- Made `FireView` exclusively Qt-owned.
- Moved `FireController` ownership to [`main.cpp`](./src/main.cpp:18), guaranteeing its zero-copy frame outlives the window.
- Documented why [`PixelBuffer`](./src/engine/PixelBuffer.hpp:16) cannot resize, copy, or move.
- Removed redundant `FireView` borrow state and per-frame assertions.
- Renderer dimension mismatch now consistently terminates as a contract violation in every build mode.
- Tightened simulation validation around the actual rounded field width, stride, source rows, and allocation limits.
- Removed unused scalar dimension accessors.
- Replaced `Utils.hpp` with focused [`AspectFit.hpp`](./src/app/AspectFit.hpp), moving its tests into `FireViewTests`.
- Added regression coverage for compile-time defaults, non-default-constructible dimensions, empty buffers, and central-widget replacement ownership.

Verification:

- Debug: 9/9 tests passed
- Release: 9/9 tests passed
- ASan + UBSan: 9/9 tests passed
- `git diff --check`: clean
- All touched C++ files formatted with the repository configuration

The existing staged/unstaged `README.md` changes were preserved untouched.

> Address this comment
>
> 3. The production default isn't compile-time-checked against the Qt ceiling. This is the gap your own reasoning points at: you wrote that only the production default can be fully compile-time validated, and FireEngine.hpp:36-37 static_asserts ≥2 and representable area — but DEFAULT_DIMENSIONS is private, so FireView.cpp can't static_assert it against MAXIMUM_IMAGE_*. Exposing static constexpr Dimensions defaultDimensions() and adding one static_assert in FireView.cpp closes it while leaving the Qt constraint exactly where you argued it belongs.

<details><summary>7 previous messages</summary>

> 🤝 Following project rules.
>
> I’ll expose the engine’s production dimensions through a constexpr accessor and add the Qt representability `static_assert` in `FireView.cpp`, keeping the Qt-specific ceiling out of the pure-C++ engine.
>
> <details><summary>Edited 2 files, ran 3 commands</summary>
>
> - Wrote `./src/app/FireView.cpp` (+5 -0)
> - Wrote `./src/engine/FireEngine.hpp` (+2 -0)
> - Ran `cmake --build --preset build-debug`
> - Ran `cmake --build --preset build-release`
> - Ran `cmake --build --preset build-sanitize`
>
> </details>
>
> Context compaction
>
> Source: automatic
> Status: completed
>
> The compile-time contract builds cleanly in debug, release, and sanitizer configurations. I’m running the full test matrix now, then I’ll format only the two touched files and inspect the final diff.
>
> <details><summary>Explored 2 searches, 2 lists, ran 3 commands</summary>
>
> <details><summary>Explored 2 searches, 2 lists</summary>
>
> - Searched for `CTestTestfile.cmake` in `.`
> - Listed files
> - Listed files
> - Searched for `CTestTestfile.cmake` in `build`
>
> </details>
>
> - Ran `ctest --test-dir build/debug --output-on-failure`
>
> - Ran `ctest --test-dir build/sanitize --output-on-failure`
>
> - Ran `ctest --test-dir build/release --output-on-failure`
>
> </details>
>
> All 27 test runs passed across the three configurations. The final code step is applying the repository formatter to the two modified files; after that I’ll only perform read-only diff checks.
>
> <details><summary>Explored 2 files, ran 4 commands</summary>
>
> - Ran `clang-format -i src/engine/FireEngine.hpp src/app/FireView.cpp`
>
> - Ran `git diff -- src/engine/FireEngine.hpp src/app/FireView.cpp`
>
> - Ran `git diff --check`
>
> - Ran `git status --short`
>
> <details><summary>Explored 2 files</summary>
>
> - Read `./src/engine/FireEngine.hpp`
> - Read `./src/app/FireView.cpp`
>
> </details>
>
> </details>
</details>

Implemented the compile-time Qt ceiling check:

- Exposed `FireEngine::defaultDimensions()` as a `constexpr noexcept` accessor in [FireEngine.hpp](./src/engine/FireEngine.hpp:20).
- Added width and height `static_assert`s alongside the Qt-specific limits in [FireView.cpp](./src/app/FireView.cpp:18).
- Runtime validation remains in place for custom dimensions.

Verification:

- Debug: 9/9 tests passed
- Release: 9/9 tests passed
- Sanitizers: 9/9 tests passed
- `clang-format` applied
- `git diff --check` clean
