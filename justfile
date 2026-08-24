default: run-release

configure-debug:
    cmake --fresh --preset cfg-debug

configure-release:
    cmake --fresh --preset cfg-release

configure-profile:
    cmake --fresh --preset cfg-profile

build-debug: configure-debug
    cmake --build --preset build-debug

build-release: configure-release
    cmake --build --preset build-release

build-profile: configure-profile
    cmake --build --preset build-profile

build-all: build-debug build-release build-profile

check-format:
    clang-format --dry-run --Werror `rg --files src -g '*.{c,cc,cpp,cxx,h,hh,hpp,hxx}'`

run-release: build-release
    cmake --build --preset build-release --target run_app
