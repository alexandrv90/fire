cmake_minimum_required(VERSION 3.25)

project(ClassicFireDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(fire_simulation
    src/sim/FireParameters.cpp
    src/sim/FireParameters.hpp
    src/sim/FireSimulation.cpp
    src/sim/FireSimulation.hpp
)

target_include_directories(fire_simulation PUBLIC src)
target_compile_features(fire_simulation PUBLIC cxx_std_20)

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

if(MSVC)
    target_compile_options(fire_simulation PRIVATE /W4 /permissive-)
    target_compile_options(classic_fire PRIVATE /W4 /permissive-)
else()
    target_compile_options(fire_simulation PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(classic_fire PRIVATE -Wall -Wextra -Wpedantic)
endif()
