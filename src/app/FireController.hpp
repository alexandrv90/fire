#pragma once

#include "engine/FireEngine.hpp"
#include "sim/Dimensions.hpp"

#include <QObject>
#include <QTimer>

#include <chrono>

class FireController final : public QObject {
    Q_OBJECT

public:
    explicit FireController(QObject* parent = nullptr);
    explicit FireController(Dimensions dimensions, QObject* parent = nullptr);

    [[nodiscard]] bool isRunning() const noexcept { return wakeTimer.isActive(); }
    [[nodiscard]] bool isRunRequested() const noexcept { return runRequested; }
    [[nodiscard]] bool isSuspended() const noexcept { return suspensionActive; }
    [[nodiscard]] const PixelBuffer& frame() const noexcept { return engine.frame(); }
    [[nodiscard]] FirePalettePresetId palettePreset() const noexcept { return engine.palettePreset(); }
    [[nodiscard]] const FireParameters& parameters() const noexcept { return engine.parameters(); }

public slots:
    void run();
    void pause();
    void toggleRunning();
    void reset();
    void setPalettePreset(FirePalettePresetId preset) noexcept;
    void setParameters(const FireParameters& parameters);
    void setMetricsEnabled(bool enabled) noexcept;
    void setSuspended(bool suspended);

signals:
    void frameReady();
    void advanceMeasured(FrameReport report);
    void wakeMeasured(std::chrono::steady_clock::time_point now);
    void parametersChanged(FireParameters parameters);
    void runningChanged(bool running);

private slots:
    void onWake() noexcept;

private:
    using Clock = std::chrono::steady_clock;

    static constexpr int WAKE_INTERVAL_MILLISECONDS = 16;

    void initializeWakeTimer();
    void updateWakeTimer();

    FireEngine engine;
    QTimer wakeTimer;
    Clock::time_point lastWake;
    bool metricsEnabled{false};
    bool runRequested{false};
    bool suspensionActive{false};
};
