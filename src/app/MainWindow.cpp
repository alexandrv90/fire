#include "app/MainWindow.hpp"

#include "app/ControlPanel.hpp"
#include "app/FireController.hpp"
#include "app/FireWidget.hpp"

#include <QKeySequence>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Classic Palette Fire"));

    auto* const centralWidget = new QWidget(this);
    auto* const windowLayout = new QVBoxLayout(centralWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    fireController = new FireController(SIMULATION_WIDTH, SIMULATION_HEIGHT, this);
    fireWidget = new FireWidget(SIMULATION_WIDTH, SIMULATION_HEIGHT, centralWidget);
    windowLayout->addWidget(fireWidget, 1);

    controlPanel = new ControlPanel(fireController->parameters(), centralWidget);
    windowLayout->addWidget(controlPanel);
    setCentralWidget(centralWidget);

    connect(controlPanel, &ControlPanel::toggleRequested, fireController, &FireController::toggleRunning);
    connect(controlPanel, &ControlPanel::resetRequested, fireController, &FireController::reset);
    connect(controlPanel, &ControlPanel::sourceHeatChanged, fireController, &FireController::setSourceHeat);
    connect(controlPanel, &ControlPanel::coolingChanged, fireController, &FireController::setCooling);
    connect(controlPanel, &ControlPanel::windChanged, fireController, &FireController::setWind);
    connect(fireController, &FireController::frameReady, this, &MainWindow::presentFrame);
    connect(fireController, &FireController::runningChanged, this, &MainWindow::updateRunningState);

    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    connect(pauseShortcut, &QShortcut::activated, fireController, &FireController::toggleRunning);
    connect(resetShortcut, &QShortcut::activated, fireController, &FireController::reset);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    presentFrame();
    fireController->run();
    resize(960, 720);
}

void MainWindow::presentFrame() { fireWidget->present(fireController->heat()); }

void MainWindow::updateRunningState(const bool running) {
    controlPanel->setPaused(!running);
    fireWidget->setPaused(!running);
}
