#include "app/FireController.hpp"

FireController::FireController(const std::size_t simulationWidth,
                               const std::size_t simulationHeight,
                               QObject* const parent)
    : QObject(parent), simulation(simulationWidth, simulationHeight), renderer(FirePalette::classic()) {
    renderer.render(simulation.heat());
    wakeTimer.setTimerType(Qt::PreciseTimer);
    wakeTimer.setInterval(WAKE_INTERVAL_MILLISECONDS);
    connect(&wakeTimer, &QTimer::timeout, this, &FireController::advanceFrame);
}

void FireController::run() {
    if (isRunning()) {
        return;
    }

    elapsedTimeReference = Clock::now();
    wakeTimer.start();
    emit runningChanged(true);
}

void FireController::pause() {
    if (!isRunning()) {
        return;
    }

    wakeTimer.stop();
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
    renderer.render(simulation.heat());
    emit frameReady();
}

void FireController::setSourceHeat(const int sourceHeat) {
    simulation.parameters().setSourceHeat(static_cast<std::uint8_t>(sourceHeat));
}

void FireController::setCooling(const int cooling) {
    simulation.parameters().setCooling(static_cast<std::uint8_t>(cooling));
}

void FireController::advanceFrame() {
    const auto now = Clock::now();
    accumulatedTime += now - elapsedTimeReference;
    elapsedTimeReference = now;

    const auto maximumCatchUpTime = SIMULATION_STEP * MAX_TICKS_PER_WAKE;
    if (accumulatedTime > maximumCatchUpTime) {
        accumulatedTime = maximumCatchUpTime;
    }

    int ticksExecuted = 0;
    while (accumulatedTime >= SIMULATION_STEP && ticksExecuted < MAX_TICKS_PER_WAKE) {
        simulation.tick();
        accumulatedTime -= SIMULATION_STEP;
        ++ticksExecuted;
    }

    if (ticksExecuted > 0) {
        renderer.render(simulation.heat());
        emit frameReady();
    }
}
