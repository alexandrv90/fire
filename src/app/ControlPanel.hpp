#pragma once

#include "sim/FireParameters.hpp"

#include <QWidget>

class QPushButton;
class QSlider;

class ControlPanel final : public QWidget {
    Q_OBJECT

public:
    explicit ControlPanel(const FireParameters& parameters, QWidget* parent = nullptr);

    void setPaused(bool paused);
    void setMetricsEnabled(bool enabled);

public slots:
    void setParameters(FireParameters parameters);

signals:
    void toggleRequested();
    void metricsEnabledChanged(bool enabled);
    void resetRequested();
    void parametersChanged(FireParameters parameters);

private:
    [[nodiscard]] FireParameters parametersFromControls() const noexcept;

    QPushButton* pauseButton{nullptr};
    QPushButton* metricsButton{nullptr};
    QSlider* sourceHeatSlider{nullptr};
    QSlider* coolingSlider{nullptr};
};
