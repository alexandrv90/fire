#pragma once

#include "sim/FireSimulation.hpp"

#include <QObject>
#include <QTimer>

#include <cstddef>
#include <cstdint>
#include <span>

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(std::size_t simulationWidth, std::size_t simulationHeight, QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return frameClock.isActive(); }
    [[nodiscard]] std::span<const std::uint8_t> heat() const noexcept { return simulation.heat(); }
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
    static constexpr int FRAME_INTERVAL_MILLISECONDS = 16;

    FireSimulation simulation;
    QTimer frameClock;
};
