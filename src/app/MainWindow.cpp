#include "app/MainWindow.hpp"

#include "app/ControlPanel.hpp"
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

    fireWidget = new FireWidget(SIMULATION_WIDTH, SIMULATION_HEIGHT, centralWidget);
    windowLayout->addWidget(fireWidget, 1);

    controlPanel = new ControlPanel(simulation.parameters(), centralWidget);
    windowLayout->addWidget(controlPanel);
    setCentralWidget(centralWidget);

    connect(controlPanel, &ControlPanel::pauseRequested, this, &MainWindow::togglePaused);
    connect(controlPanel, &ControlPanel::resetRequested, this, &MainWindow::resetSimulation);
    connect(controlPanel, &ControlPanel::sourceHeatChanged, this, &MainWindow::setSourceHeat);
    connect(controlPanel, &ControlPanel::coolingChanged, this, &MainWindow::setCooling);
    connect(controlPanel, &ControlPanel::windChanged, this, &MainWindow::setWind);

    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    connect(pauseShortcut, &QShortcut::activated, this, &MainWindow::togglePaused);
    connect(resetShortcut, &QShortcut::activated, this, &MainWindow::resetSimulation);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    frameTimer.setTimerType(Qt::PreciseTimer);
    frameTimer.setInterval(16);
    connect(&frameTimer, &QTimer::timeout, this, &MainWindow::advanceFrame);

    fireWidget->present(simulation.heat());
    frameTimer.start();
    resize(960, 720);
}

void MainWindow::advanceFrame() {
    if (isPaused) {
        return;
    }
    simulation.tick();
    fireWidget->present(simulation.heat());
}

void MainWindow::togglePaused() { setPaused(!isPaused); }

void MainWindow::resetSimulation() {
    simulation.reset();
    fireWidget->present(simulation.heat());
}

void MainWindow::setPaused(const bool paused) {
    isPaused = paused;
    controlPanel->setPaused(isPaused);
    fireWidget->setPaused(isPaused);
}

void MainWindow::setSourceHeat(const int sourceHeat) {
    simulation.parameters().setSourceHeat(static_cast<std::uint8_t>(sourceHeat));
}

void MainWindow::setCooling(const int cooling) {
    simulation.parameters().setCooling(static_cast<std::uint8_t>(cooling));
}

void MainWindow::setWind(const int wind) { simulation.parameters().setWind(wind); }
