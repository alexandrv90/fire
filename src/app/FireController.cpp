#include "app/FireController.hpp"

FireController::FireController(QObject* const parent) : QObject(parent) {
    wakeTimer.setTimerType(Qt::PreciseTimer);
    wakeTimer.setInterval(WAKE_INTERVAL_MILLISECONDS);
    connect(&wakeTimer, &QTimer::timeout, this, &FireController::onWake);
}

void FireController::run() {
    if (isRunning()) {
        return;
    }

    lastWake = Clock::now();
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
    engine.reset();
    if (isRunning()) {
        lastWake = Clock::now();
    }

    emit parametersChanged(engine.parameters());
    emit frameReady(FrameReport{});
}

void FireController::setParameters(const FireParameters& parameters) {
    engine.setParameters(parameters);
    emit parametersChanged(engine.parameters());
}

void FireController::setMetricsEnabled(const bool enabled) noexcept {
    metricsEnabled = enabled;
    engine.setStageTimingEnabled(enabled);
}

void FireController::onWake() {
    const auto now = Clock::now();
    const auto elapsed = now - lastWake;
    lastWake = now;
    if (metricsEnabled) {
        emit wakeMeasured(now);
    }

    const FrameReport report = engine.advance(elapsed);
    if (report.ticksExecuted > 0) {
        if (report.stageTimings.has_value()) {
            emit frameMeasured(report);
        }
        emit frameReady(report);
    }
}
