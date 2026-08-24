#include "app/MainWindow.hpp"

#include "app/FireWidget.hpp"

#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

namespace {
QLabel* makeValueLabel(QWidget* const parent, const int initialValue) {
    auto* const label = new QLabel(QString::number(initialValue), parent);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMinimumWidth(28);
    return label;
}

void addSlider(QHBoxLayout& layout,
               QWidget* const parent,
               const QString& title,
               const int minimum,
               const int maximum,
               const int initialValue,
               const std::function<void(int)>& applyValue) {
    auto* const titleLabel = new QLabel(title, parent);
    auto* const slider = new QSlider(Qt::Horizontal, parent);
    auto* const valueLabel = makeValueLabel(parent, initialValue);
    slider->setRange(minimum, maximum);
    slider->setValue(initialValue);
    slider->setMinimumWidth(90);

    QObject::connect(slider, &QSlider::valueChanged, parent, [valueLabel, applyValue](const int value) {
        valueLabel->setNum(value);
        applyValue(value);
    });

    layout.addWidget(titleLabel);
    layout.addWidget(slider, 1);
    layout.addWidget(valueLabel);
}
} // namespace

MainWindow::MainWindow(QWidget* const parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Classic Palette Fire"));

    auto* const centralWidget = new QWidget(this);
    auto* const windowLayout = new QVBoxLayout(centralWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    fireWidget = new FireWidget(SIMULATION_WIDTH, SIMULATION_HEIGHT, centralWidget);
    windowLayout->addWidget(fireWidget, 1);

    auto* const controls = new QWidget(centralWidget);
    auto* const controlsLayout = new QHBoxLayout(controls);
    controlsLayout->setContentsMargins(12, 8, 12, 8);

    pauseButton = new QPushButton(QStringLiteral("Pause"), controls);
    auto* const resetButton = new QPushButton(QStringLiteral("Reset"), controls);
    pauseButton->setToolTip(QStringLiteral("Pause or resume (Space)"));
    resetButton->setToolTip(QStringLiteral("Restart the simulation (R)"));
    controlsLayout->addWidget(pauseButton);
    controlsLayout->addWidget(resetButton);
    controlsLayout->addSpacing(8);

    addSlider(
        *controlsLayout, controls, QStringLiteral("Heat"), 32, 255, simulation.sourceHeat(), [this](const int value) {
            simulation.setSourceHeat(static_cast<std::uint8_t>(value));
        });
    controlsLayout->addSpacing(8);
    addSlider(*controlsLayout,
              controls,
              QStringLiteral("Cooling"),
              0,
              FireSimulation::MAXIMUM_COOLING,
              simulation.cooling(),
              [this](const int value) { simulation.setCooling(static_cast<std::uint8_t>(value)); });
    controlsLayout->addSpacing(8);
    addSlider(*controlsLayout,
              controls,
              QStringLiteral("Wind"),
              FireSimulation::MINIMUM_WIND,
              FireSimulation::MAXIMUM_WIND,
              simulation.wind(),
              [this](const int value) { simulation.setWind(value); });

    windowLayout->addWidget(controls);
    setCentralWidget(centralWidget);

    connect(pauseButton, &QPushButton::clicked, this, [this] { togglePaused(); });
    connect(resetButton, &QPushButton::clicked, this, [this] { resetSimulation(); });

    auto* const pauseShortcut = new QShortcut(QKeySequence{Qt::Key_Space}, this);
    auto* const resetShortcut = new QShortcut(QKeySequence{Qt::Key_R}, this);
    auto* const quitShortcut = new QShortcut(QKeySequence{Qt::Key_Escape}, this);
    connect(pauseShortcut, &QShortcut::activated, this, [this] { togglePaused(); });
    connect(resetShortcut, &QShortcut::activated, this, [this] { resetSimulation(); });
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    frameTimer.setTimerType(Qt::PreciseTimer);
    frameTimer.setInterval(16);
    connect(&frameTimer, &QTimer::timeout, this, [this] { advanceFrame(); });

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
    pauseButton->setText(isPaused ? QStringLiteral("Resume") : QStringLiteral("Pause"));
    fireWidget->setPaused(isPaused);
}
