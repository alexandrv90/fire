#pragma once

#include "sim/FireSimulation.hpp"

#include <QObject>
#include <QTimer>

#include <chrono>
#include <cstddef>

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(std::size_t simulationWidth, std::size_t simulationHeight, QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    [[nodiscard]] HeatFrame heat() const noexcept { return simulation.heat(); }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return simulation.parameters(); }

public slots:
    void run();
    void pause();
    void toggleRunning();
    void reset();
    void setSourceHeat(int sourceHeat);
    void setCooling(int cooling);

signals:
    void frameReady();
    void runningChanged(bool running);

private slots:
    void advanceFrame();

private:
    using Clock = std::chrono::steady_clock;

    static constexpr int WAKE_INTERVAL_MILLISECONDS = 16;
    static constexpr int SIMULATION_TICKS_PER_SECOND = 60;
    static constexpr int MAX_TICKS_PER_WAKE = 3;
    static constexpr std::chrono::duration<double> SIMULATION_STEP{1.0 / SIMULATION_TICKS_PER_SECOND};

    FireSimulation simulation;
    QTimer wakeTimer;
    Clock::time_point elapsedTimeReference;
    std::chrono::duration<double> accumulatedTime{0.0};
};
