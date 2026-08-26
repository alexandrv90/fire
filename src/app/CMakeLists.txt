find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(qtWidgetsTarget "Qt${QT_VERSION_MAJOR}::Widgets")

fire_add_library(fire_app
    AUTOMOC
    STATIC
    SOURCES
        ControlPanel.cpp
        ControlPanel.hpp
        FireController.cpp
        FireController.hpp
        FireView.cpp
        FireView.hpp
        FrameMetricsCollector.cpp
        FrameMetricsCollector.hpp
        MainWindow.cpp
        MainWindow.hpp
        StatsPanel.cpp
        StatsPanel.hpp
    PUBLIC_DEPENDENCIES
        fire_engine
        fire_metrics
        ${qtWidgetsTarget}
)

fire_add_executable(fire_demo
    SOURCES
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
    DEPENDENCIES
        fire_app
)

set_target_properties(fire_demo PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Classic Fire"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.classicfire"
    WIN32_EXECUTABLE TRUE
)

add_custom_target(run_app
    COMMAND "$<TARGET_FILE:fire_demo>"
    DEPENDS fire_demo
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:fire_demo>"
    USES_TERMINAL
    COMMENT "Starting Fire"
)

fire_add_test(fire_app_metrics_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/app/FrameMetricsCollectorTests.cpp"
    DEPENDENCIES
        fire_app
)

fire_add_test(fire_app_controller_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/app/FireControllerTests.cpp"
    DEPENDENCIES
        fire_app
)

fire_add_test(fire_app_control_panel_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/app/ControlPanelTests.cpp"
    DEPENDENCIES
        fire_app
    ENVIRONMENT
        QT_QPA_PLATFORM=offscreen
)

fire_add_test(fire_app_stats_panel_tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/tests/app/StatsPanelTests.cpp"
    DEPENDENCIES
        fire_app
    ENVIRONMENT
        QT_QPA_PLATFORM=offscreen
)
