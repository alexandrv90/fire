#include "app/MainWindow.hpp"

#include "app/ControlPanel.hpp"
#include "app/FireController.hpp"
#include "app/FireView.hpp"
#include "app/FrameMetricsCollector.hpp"
#include "app/StatsPanel.hpp"

#include <QEvent>
#include <QKeySequence>
#include <QShortcut>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Fire Demo"));

    auto* const centralWidget = new QWidget(this);
    auto* const windowLayout = new QVBoxLayout(centralWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    auto* const frameMetricsCollector = new FrameMetricsCollector(this);
    fireController = new FireController(this);

    auto* const renderArea = new QWidget(centralWidget);
    auto* const renderStack = new QStackedLayout(renderArea);
    renderStack->setContentsMargins(0, 0, 0, 0);
    renderStack->setStackingMode(QStackedLayout::StackAll);

    auto* const fireView = new FireView(fireController->frame(), renderArea);
    renderStack->addWidget(fireView);

    auto* const overlayLayer = new QWidget(renderArea);
    overlayLayer->setAttribute(Qt::WA_NoSystemBackground);
    overlayLayer->setAttribute(Qt::WA_TransparentForMouseEvents);
    auto* const overlayLayout = new QVBoxLayout(overlayLayer);
    overlayLayout->setContentsMargins(12, 12, 12, 12);
    overlayLayout->setSpacing(0);

    statsPanel = new StatsPanel(*frameMetricsCollector, overlayLayer);
    overlayLayout->addWidget(statsPanel, 0, Qt::AlignLeft | Qt::AlignTop);
    overlayLayout->addStretch(1);
    renderStack->addWidget(overlayLayer);
    renderStack->setCurrentWidget(overlayLayer);

    windowLayout->addWidget(renderArea, 1);

    auto* const controlPanel =
        new ControlPanel(fireController->parameters(), fireController->palettePreset(), centralWidget);
    windowLayout->addWidget(controlPanel);
    setCentralWidget(centralWidget);
    // clang-format off
    connect(controlPanel, &ControlPanel::toggleRequested, fireController, &FireController::toggleRunning);
    connect(controlPanel, &ControlPanel::metricsEnabledChanged, frameMetricsCollector, &FrameMetricsCollector::setEnabled);
    connect(controlPanel, &ControlPanel::resetRequested, fireController, &FireController::reset);
    connect(controlPanel, &ControlPanel::palettePresetChanged, fireController, &FireController::setPalettePreset);
    connect(controlPanel, &ControlPanel::parametersChanged, fireController, &FireController::setParameters);

    connect(fireController, &FireController::parametersChanged, controlPanel, &ControlPanel::setParameters);
    connect(fireController, &FireController::frameReady, fireView, &FireView::present);
    connect(fireController, &FireController::runningChanged, controlPanel,
        [controlPanel](const bool running) {controlPanel->setPaused(!running);});

    connect(frameMetricsCollector, &FrameMetricsCollector::enabledChanged, fireController, &FireController::setMetricsEnabled);
    connect(frameMetricsCollector, &FrameMetricsCollector::enabledChanged, fireView, &FireView::setMetricsEnabled);
    connect(frameMetricsCollector, &FrameMetricsCollector::enabledChanged, controlPanel, &ControlPanel::setMetricsEnabled);
    connect(fireController, &FireController::wakeMeasured, frameMetricsCollector, &FrameMetricsCollector::observeWake);
    connect(fireController, &FireController::advanceMeasured, frameMetricsCollector, &FrameMetricsCollector::observeAdvance);
    connect(fireView, &FireView::paintMeasured, frameMetricsCollector, &FrameMetricsCollector::observePaint);
    // clang-format on

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

void MainWindow::changeEvent(QEvent* const event) {
    QMainWindow::changeEvent(event);
    if (event->type() != QEvent::WindowStateChange) {
        return;
    }

    const bool suspended = isMinimized();
    fireController->setSuspended(suspended);
    statsPanel->setSuspended(suspended);
}
