default: run-release

configure-debug:
    cmake --preset cfg-debug

configure-release:
    cmake --preset cfg-release

configure-profile:
    cmake --preset cfg-profile

configure-sanitize:
    cmake --preset cfg-sanitize

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
