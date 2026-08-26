#pragma once

#include "engine/FireEngine.hpp"

#include <QObject>
#include <QTimer>

#include <chrono>
#include <cstddef>

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(std::size_t simulationWidth, std::size_t simulationHeight, QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    [[nodiscard]] const PixelBuffer& frame() const noexcept { return engine.frame(); }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return engine.parameters(); }
    [[nodiscard]] FrameProfiler& profiler() noexcept { return engine.profiler(); }
    [[nodiscard]] const FrameProfiler& profiler() const noexcept { return engine.profiler(); }

public slots:
    void run();
    void pause();
    void toggleRunning();
    void reset();
    void setParameters(const FireParameters& parameters);

signals:
    void frameReady(FrameReport report);
    void parametersChanged(FireParameters parameters);
    void runningChanged(bool running);

private slots:
    void onWake();

private:
    using Clock = std::chrono::steady_clock;

    static constexpr int WAKE_INTERVAL_MILLISECONDS = 16;

    FireEngine engine;
    QTimer wakeTimer;
    Clock::time_point lastWake;
};
