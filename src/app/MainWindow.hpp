#pragma once

#include "sim/FireSimulation.hpp"

#include <QMainWindow>
#include <QTimer>

class ControlPanel;
class FireWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void advanceFrame();
    void togglePaused();
    void resetSimulation();
    void setPaused(bool paused);
    void setSourceHeat(int sourceHeat);
    void setCooling(int cooling);
    void setWind(int wind);

private:
    static constexpr int SIMULATION_WIDTH = 800;
    static constexpr int SIMULATION_HEIGHT = 600;

    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT};
    FireWidget* fireWidget{nullptr};
    ControlPanel* controlPanel{nullptr};
    QTimer frameTimer{this};
    bool isPaused{false};
};
