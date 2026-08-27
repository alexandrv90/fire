#pragma once

#include "engine/FirePalette.hpp"
#include "sim/FireParameters.hpp"

#include <QWidget>

class QComboBox;
class QPushButton;
class QSlider;

class ControlPanel final : public QWidget {
    Q_OBJECT

public:
    explicit ControlPanel(const FireParameters& parameters,
                          FirePalettePresetId palettePreset = FirePalettePresetId::Classic,
                          QWidget* parent = nullptr);

    void setPaused(bool paused);
    void setMetricsEnabled(bool enabled);

public slots:
    void setPalettePreset(FirePalettePresetId preset);
    void setParameters(FireParameters parameters);

signals:
    void toggleRequested();
    void metricsEnabledChanged(bool enabled);
    void resetRequested();
    void palettePresetChanged(FirePalettePresetId preset);
    void parametersChanged(FireParameters parameters);

private:
    [[nodiscard]] FireParameters parametersFromControls() const noexcept;

    QPushButton* pauseButton{nullptr};
    QPushButton* metricsButton{nullptr};
    QComboBox* paletteComboBox{nullptr};
    QSlider* sourceHeatSlider{nullptr};
    QSlider* coolingSlider{nullptr};
};
