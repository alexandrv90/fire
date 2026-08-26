fire_add_library(fire_engine
    SOURCES
        FireEngine.cpp
        FireEngine.hpp
        FrameClock.cpp
        FrameClock.hpp
        FrameReport.hpp
    PUBLIC_DEPENDENCIES
        fire_simulation
        fire_render
)

fire_add_test(fire_engine_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/engine/FrameClockTests.cpp"
    DEPENDENCIES
        fire_engine
)
