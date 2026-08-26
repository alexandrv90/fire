#pragma once

#include "engine/FireEngine.hpp"

#include <QObject>
#include <QTimer>

#include <chrono>

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    [[nodiscard]] const PixelBuffer& frame() const noexcept { return engine.frame(); }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return engine.parameters(); }

public slots:
    void run();
    void pause();
    void toggleRunning();
    void reset();
    void setParameters(const FireParameters& parameters);
    void setMetricsEnabled(bool enabled) noexcept;

signals:
    void frameReady(FrameReport report);
    void frameMeasured(FrameReport report);
    void wakeMeasured(std::chrono::steady_clock::time_point now);
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
    bool metricsEnabled{false};
};
