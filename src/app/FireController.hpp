#pragma once

#include "engine/FrameClock.hpp"
#include "render/FireRenderer.hpp"
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
    [[nodiscard]] const PixelBuffer& frame() const noexcept { return renderer.target(); }
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

    FireSimulation simulation;
    FireRenderer renderer;
    FrameClock frameClock{SIMULATION_TICKS_PER_SECOND, MAX_TICKS_PER_WAKE};
    QTimer wakeTimer;
    Clock::time_point elapsedTimeReference;
};
