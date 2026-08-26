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

public slots:
    void setParameters(FireParameters parameters);

signals:
    void toggleRequested();
    void resetRequested();
    void parametersChanged(FireParameters parameters);

private:
    [[nodiscard]] FireParameters parametersFromControls() const noexcept;

    QPushButton* pauseButton{nullptr};
    QSlider* sourceHeatSlider{nullptr};
    QSlider* coolingSlider{nullptr};
};
