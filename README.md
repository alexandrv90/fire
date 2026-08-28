# Fire Demo

A C++20 and Qt Widgets demo of the classic procedural fire effect, with live simulation controls.

Target platforms: **Linux/macOS**

Supported features:

- Three selectable fire palettes
- Heat control
- Cooling (fire decay) control
- Pause/Reset animation
- Live metrics display
- Suppend activity when minimized

## Prerequisites

- A C++20 compiler
- Qt 5 or Qt 6 with Widgets
- CMake 3.25+
- Ninja (required only when using the provided presets)

### Build and run without a preset

```sh
cmake -S . -B build/local -D CMAKE_BUILD_TYPE=Release
cmake --build build/local --config Release --target run_app
```

### Optional: build with `just` tool if available

```sh
just run-release
```

## Other deliverables

- Writeup: [report/report.md](report/report.md)
- AI transcripts: [report/transcripts](report/trascripts/)


