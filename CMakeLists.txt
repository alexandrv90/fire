cmake_minimum_required(VERSION 3.25)

project(ClassicFireDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(cmake/Sanitizers.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(fire_simulation
    src/sim/FireParameters.cpp
    src/sim/FireParameters.hpp
    src/sim/FireSimulation.cpp
    src/sim/FireSimulation.hpp
    src/sim/HeatFrame.hpp
)

target_include_directories(fire_simulation PUBLIC src)
target_compile_features(fire_simulation PUBLIC cxx_std_20)
fire_enable_sanitizers(fire_simulation)

add_library(fire_render
    src/render/FirePalette.cpp
    src/render/FirePalette.hpp
    src/render/FireRenderer.cpp
    src/render/FireRenderer.hpp
    src/render/PixelBuffer.cpp
    src/render/PixelBuffer.hpp
    src/render/Viewport.hpp
)

target_include_directories(fire_render PUBLIC src)
target_compile_features(fire_render PUBLIC cxx_std_20)
target_link_libraries(fire_render PUBLIC fire_simulation)
fire_enable_sanitizers(fire_render)

add_library(fire_metrics
    src/metrics/FrameProfiler.cpp
    src/metrics/FrameProfiler.hpp
    src/metrics/IntervalMetric.cpp
    src/metrics/IntervalMetric.hpp
    src/metrics/MetricStatistics.hpp
    src/metrics/ScopedTimer.cpp
    src/metrics/ScopedTimer.hpp
    src/metrics/TimeSeriesMetric.cpp
    src/metrics/TimeSeriesMetric.hpp
)

target_include_directories(fire_metrics PUBLIC src)
target_compile_features(fire_metrics PUBLIC cxx_std_20)
fire_enable_sanitizers(fire_metrics)

add_executable(classic_fire
    src/app/ControlPanel.cpp
    src/app/ControlPanel.hpp
    src/app/FireController.cpp
    src/app/FireController.hpp
    src/app/FireWidget.cpp
    src/app/FireWidget.hpp
    src/app/MainWindow.cpp
    src/app/MainWindow.hpp
    src/main.cpp
)

target_include_directories(classic_fire PRIVATE src)

target_link_libraries(classic_fire PRIVATE
    fire_simulation
    Qt${QT_VERSION_MAJOR}::Widgets
)
fire_enable_sanitizers(classic_fire)

set_target_properties(classic_fire PROPERTIES
    AUTOMOC TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Classic Fire"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.classicfire"
    WIN32_EXECUTABLE TRUE
)

add_custom_target(run_app
    COMMAND "$<TARGET_FILE:classic_fire>"
    DEPENDS classic_fire
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:classic_fire>"
    USES_TERMINAL
    COMMENT "Starting Classic Fire"
)

if(BUILD_TESTING)
    add_executable(fire_metrics_tests
        tests/metrics/MetricsTests.cpp
    )

    target_link_libraries(fire_metrics_tests PRIVATE fire_metrics)
    fire_enable_sanitizers(fire_metrics_tests)

    add_test(NAME fire_metrics_tests COMMAND fire_metrics_tests)

    add_executable(fire_simulation_tests
        tests/sim/FireSimulationTests.cpp
    )

    target_link_libraries(fire_simulation_tests PRIVATE fire_simulation)
    fire_enable_sanitizers(fire_simulation_tests)

    add_test(NAME fire_simulation_tests COMMAND fire_simulation_tests)

    add_executable(fire_render_tests
        tests/render/RenderTests.cpp
    )

    target_link_libraries(fire_render_tests PRIVATE fire_render)
    fire_enable_sanitizers(fire_render_tests)

    add_test(NAME fire_render_tests COMMAND fire_render_tests)
endif()

if(MSVC)
    target_compile_options(fire_simulation PRIVATE /W4 /permissive-)
    target_compile_options(fire_render PRIVATE /W4 /permissive-)
    target_compile_options(fire_metrics PRIVATE /W4 /permissive-)
    target_compile_options(classic_fire PRIVATE /W4 /permissive-)
    if(BUILD_TESTING)
        target_compile_options(fire_metrics_tests PRIVATE /W4 /permissive-)
        target_compile_options(fire_simulation_tests PRIVATE /W4 /permissive-)
        target_compile_options(fire_render_tests PRIVATE /W4 /permissive-)
    endif()
else()
    target_compile_options(fire_simulation PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(fire_render PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(fire_metrics PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(classic_fire PRIVATE -Wall -Wextra -Wpedantic)
    if(BUILD_TESTING)
        target_compile_options(fire_metrics_tests PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(fire_simulation_tests PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(fire_render_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
