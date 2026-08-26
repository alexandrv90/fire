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

