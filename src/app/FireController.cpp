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
    const TickPlan plan = frameClock.consume(now - elapsedTimeReference);
    elapsedTimeReference = now;

    for (int tick = 0; tick < plan.ticks; ++tick) {
        simulation.tick();
    }

    if (plan.ticks > 0) {
        renderer.render(simulation.heat());
        emit frameReady();
    }
}
