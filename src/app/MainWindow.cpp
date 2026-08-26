#include "app/MainWindow.hpp"

#include "app/ControlPanel.hpp"
#include "app/FireController.hpp"
#include "app/FireView.hpp"
#include "app/FrameMetricsCollector.hpp"

#include <QKeySequence>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Fire Demo"));

    auto* const centralWidget = new QWidget(this);
    auto* const windowLayout = new QVBoxLayout(centralWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    auto* const frameMetricsCollector = new FrameMetricsCollector(this);
    auto* const fireController = new FireController(SIMULATION_WIDTH, SIMULATION_HEIGHT, this);
    auto* const fireView = new FireView(centralWidget);
    fireView->present(fireController->frame());
    windowLayout->addWidget(fireView, 1);

    auto* const controlPanel = new ControlPanel(fireController->parameters(), centralWidget);
    windowLayout->addWidget(controlPanel);
    setCentralWidget(centralWidget);

    connect(controlPanel, &ControlPanel::toggleRequested, fireController, &FireController::toggleRunning);
    connect(controlPanel, &ControlPanel::resetRequested, fireController, &FireController::reset);
    connect(controlPanel, &ControlPanel::parametersChanged, fireController, &FireController::setParameters);
    connect(fireController, &FireController::parametersChanged, controlPanel, &ControlPanel::setParameters);
    connect(fireController, &FireController::frameReady, fireView, [fireController, fireView](const FrameReport&) {
        fireView->present(fireController->frame());
    });
    connect(fireController, &FireController::runningChanged, controlPanel, [controlPanel](const bool running) {
        controlPanel->setPaused(!running);
    });

    connect(frameMetricsCollector,
            &FrameMetricsCollector::enabledChanged,
            fireController,
            &FireController::setMetricsEnabled);
    connect(frameMetricsCollector, &FrameMetricsCollector::enabledChanged, fireView, &FireView::setMetricsEnabled);
    connect(fireController, &FireController::wakeMeasured, frameMetricsCollector, &FrameMetricsCollector::observeWake);
    connect(
        fireController, &FireController::frameMeasured, frameMetricsCollector, &FrameMetricsCollector::observeFrame);
    connect(fireView, &FireView::paintMeasured, frameMetricsCollector, &FrameMetricsCollector::observePaint);

    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    connect(pauseShortcut, &QShortcut::activated, fireController, &FireController::toggleRunning);
    connect(resetShortcut, &QShortcut::activated, fireController, &FireController::reset);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    frameMetricsCollector->setEnabled(true);
    fireController->run();
    resize(960, 720);
}
