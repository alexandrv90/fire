#pragma once

#include <QWidget>

class FireParameters;
class QPushButton;

class ControlPanel final : public QWidget {
    Q_OBJECT

public:
    explicit ControlPanel(const FireParameters& parameters, QWidget* parent = nullptr);

    void setPaused(bool paused);

signals:
    void toggleRequested();
    void resetRequested();
    void sourceHeatChanged(int sourceHeat);
    void coolingChanged(int cooling);

private:
    QPushButton* pauseButton{nullptr};
};
