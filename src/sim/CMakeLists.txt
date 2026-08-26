fire_add_library(fire_simulation
    SOURCES
        FireParameters.cpp
        FireParameters.hpp
        FireSimulation.cpp
        FireSimulation.hpp
        HeatFrame.hpp
)

fire_add_test(fire_simulation_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/sim/FireSimulationTests.cpp"
    DEPENDENCIES
        fire_simulation
)
