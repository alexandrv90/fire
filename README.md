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
