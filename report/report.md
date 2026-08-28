# Report

## Problem breakdown and research

Initial research showed that a procedural fire effect can be implemented with a relatively simple algorithm and does not require GPU acceleration.

The basic pipeline is:

1. Generate and update a heat map.
1. Convert heat values into pixels using a color palette.
1. Display the resulting pixel buffer.

## Prototype

Asking an agent to generate an initial prototype produced a working demo and confirmed that a CPU-only implementation was more than capable of handling the workload.

Good aspects

- The simulation library and main application were defined as separate CMake targets.
- Using a QImage to expose the rendered frame to Qt was a reasonable approach. Qt’s painting system could handle rasterization and scaling during presentation.
- Rendering into a fixed-size frame kept the simulation independent of the window size and avoided reallocating buffers when the window was resized.

Areas for improvement

- The fire animation did not look convincing.
- Animation was driven directly by a single `QTimer`. Timer callbacks are affected by event-loop scheduling and may arrive at irregular intervals, so using each callback as one simulation step could produce inconsistent animation speed and frame pacing.
- The class decomposition was weak: most responsibilities were concentrated in two large classes, making the implementation difficult to understand and test independently.
- The naive FPS counter was implemented inside the paint path and measured only paint events.

During the research `QRhiWidget` and `QOpenGLWidget` approaches has also been considered, but the final choice was made in favor of simpler `QImage` based rendering.

The proposed rendering pipeline was:

1. Simulation updates heat map
1. Renderer maps heat values through a palette into a persistent pixel buffer
1. QImage provides a zero-copy view of that buffer
1. QPainter scales and draws the image into the widget backing store
1. Qt and the window-system compositor handle final presentation

## Key design decisions:

- Animation pacing: The simulation rate is decoupled from the timer wake rate. A precise 16 ms `QTimer` is used only to wake the application and provide elapsed wall-clock time; it does not directly represent a simulation step. A fixed-step `FrameClock` accumulates that elapsed time, converts it into whole simulation ticks, and executes at most three ticks per wake. Any excess catch-up time is discarded and reported explicitly. This provides stable simulation progression despite irregular timer callbacks.

- The engine owns a persistent `PixelBuffer`, and the renderer writes shaded pixels directly into it. `FireView` constructs a borrowing `QImage` over the same storage, allowing `paintEvent()` to display the latest pixels without copying or reallocating the frame. The ownership model ensures that the engine and its pixel storage outlive the borrowing `QImage`.

- Stick to single-threaded execution. Simulation, shading, and painting all run on the Qt event-loop thread. The CPU workload is small enough that worker threads would add synchronization and lifetime complexity without a demonstrated benefit. This also makes the zero-copy buffer safe without locks.

- Separate simulation and presentation resolutions. The simulation and rendered pixel buffer use a fixed resolution. The window can have any size, while the view scales the fixed frame without changing its aspect ratio.

- Add live performance metrics display to have a better insights. A runtime metrics system records simulation, shading, painting, wake-interval, and paint-interval measurements. It also tracks discarded simulation time and idle timer wakes. Statistics are calculated over a rolling window of up to 512 observations and displayed in an overlay that refreshes independently at 4 Hz.

- Error-handling strategy: exceptions are permitted during initialization, where invalid dimensions or allocation-related failures can be reported to the user. Hot-path and steady-state operations avoid exceptions whenever possible and use noexcept interfaces where appropriate.

- No external test framework was specified in task description. To avoid adding implicit dependecy to the project the tests are implemented as standalone C++ executables using a small shared test helper and are registered with CTest.

## Decided not to build

- No generic rendering-pipeline abstraction. The project is an application supporting CPU renderer only rather than a reusable graphics library, and additional rendering passes or OpenCV post-processing were outside its scope.

- No GPU acceleration. Low-level graphics programming is outside my primary area of expertise, and the CPU implementation was already fast enough for the demo.

- No synchronization with the monitor refresh rate. Additional complexity was not justified for this CPU-rendered demo. Qt Widgets also does not provide a reliable way for the application to observe actual compositor presentation or synchronize simulation updates directly with it. The application therefore uses a 16 ms wake timer while keeping simulation progression independent through a fixed-step clock.

- No dynamic frame-buffer resizing. Resizing the window only changes the presentation area, the frame is scaled while preserving its aspect ratio. Dynamically resizing the buffers would require reallocating storage, rebuilding the borrowing QImage, and deciding how to preserve or restart simulation state. That complexity offered little value for this demo, while the fixed-resolution output remained visually acceptable.

- No fully polished, production-grade test suite. Tests were an important part of the agent verification workflow, but given the project’s scope, most test implementation was delegated to agents and received limited manual review.

## Performance evaluation

A precise Qt timer wakes every 16 ms, while the simulation advances independently at a fixed 90 Hz.

Metrics are accumulated during the runtime and displayed on stats panel. Each rolling metric retains up to 512 observations and shows average, p95, maximum values. The summary line reports running totals over the same window rather than a distribution.

Key metrics:

- Simulate: Time spent advancing the fire simulation state.
- Shade: Time spent converting the simulated heat values into colored frame pixels.
- Paint: Time spent drawing the current frame into the Qt widget.
- Paint interval: Time between the starts of consecutive paint operations. In steady state it is pinned near the 16 ms wake-timer period, growth indicates the application is no longer holding that cadence.
- Wake interval: Time between consecutive simulation timer wake-up, reflects timer pacing and scheduling delays.
- Frame: Index of the latest frame produced by the simulation.
- Dropped: Total elapsed simulation time discarded because the engine could not process it within its catch-up limits.
- Idle wakes: Number of timer wake-ups that produced no simulation tick.


Rendering pipeline:
```
Wake up timer -> Simulation tick(s) -> Shade into pixels -> `QWidget::update()` by FireView -> async/coalescing `QWidget::paintEvent()` -> `QPainter::drawImage` sync rasterization -> async -> compositor, display
```

None of these stages lets us define FPS reliably. The application never observes when a frame actually reaches the screen. Instead we observe the durations of the three main stages and the cadence of `paintEvent`. Qt schedules `paintEvent()` in response to `update()`, so its average interval should remain around 16 ms (wake up timer interval defined by the app). However, the average changes noticeably when a scheduling slot is missed entirely, causing it to shift toward a multiple of the target period.

For this app, we meet the allocated frame budget for Qt’s CPU rendering pipeline when the average paint interval is around 16 ms and the combined Simulate, Shade, and Paint time is less than 16 ms.

## AI tools

- `reports/transcripts` folder does not contain all session transcripts, as the process involved a lot session forking and many small, individual conversations in order to keep the context size manageable. The transcripts presented here represent the most typical and longer-running interactions.

- My typical process these days:
  - Break the work down into independent features
  - Ask an agent to analyze potential design options for a feature, identify important considerations and blind spots, and sometimes provide a sketch of the relevant interfaces
  - For long-horizon tasks, create a planning document containing a feature breakdown, key decisions, motivations, trade-offs, and implementation steps
  - Once the document is in place, ask agents to implement individual steps, review the resulting changes manually, optionally have another agent perform a code review. Once a step is complete, proceed to the next one in the plan
  - Treat each individual step as a separate PR, reviewing and adjusting it manually where necessary. The agent's initial solution rarely has an ideal structure, so for me the process is now more like "creating a sculpture" than "painting a picture," as it might have been a few years ago. Instead of drawing every individual stroke from the beginning, I start with an initial lump of code from the agent. I do not expect the agent to deliver a complete solution immediately and rather manually iterate over initial solution by removing, reshaping, and adjusting individual pieces. This approach helps speed up the development process, keep the most essential decisions under control and build intuition about the implementation details

- Agent tools used:
  - Codex for most of the implementation work
  - Claude Opus for design and review, as well as occasional difficult tasks. It was also able to develop and fine-tune the fire simulation model presented here based on the provided reference video

- The most common issues with generated code are poor structure and weak long-horizon planning, rather than problems with individual implementation details.
