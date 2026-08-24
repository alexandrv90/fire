#include "app/FireController.hpp"

FireController::FireController(const std::size_t simulationWidth,
                               const std::size_t simulationHeight,
                               QObject* const parent)
    : QObject(parent), simulation(simulationWidth, simulationHeight) {
    frameClock.setTimerType(Qt::PreciseTimer);
    frameClock.setInterval(FRAME_INTERVAL_MILLISECONDS);
    connect(&frameClock, &QTimer::timeout, this, &FireController::advanceFrame);
}

void FireController::run() {
    if (isRunning()) {
        return;
    }

    frameClock.start();
    emit runningChanged(true);
}

void FireController::pause() {
    if (!isRunning()) {
        return;
    }

    frameClock.stop();
    emit runningChanged(false);
}

void FireController::toggleRunning() {
    if (isRunning()) {
        pause();
    } else {
        run();
    }
}

void FireController::reset() {
    simulation.reset();
    emit frameReady();
}

void FireController::setSourceHeat(const int sourceHeat) {
    simulation.parameters().setSourceHeat(static_cast<std::uint8_t>(sourceHeat));
}

void FireController::setCooling(const int cooling) {
    simulation.parameters().setCooling(static_cast<std::uint8_t>(cooling));
}

void FireController::advanceFrame() {
    simulation.tick();
    emit frameReady();
}
