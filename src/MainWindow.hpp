#pragma once

#include "FireSimulation.hpp"

#include <QMainWindow>
#include <QTimer>

class FireWidget;
class QPushButton;

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void advanceFrame();
    void togglePaused();
    void resetSimulation();
    void setPaused(bool paused);

    static constexpr int SIMULATION_WIDTH = 800;
    static constexpr int SIMULATION_HEIGHT = 600;

    FireSimulation simulation{SIMULATION_WIDTH, SIMULATION_HEIGHT};
    FireWidget* fireWidget{nullptr};
    QPushButton* pauseButton{nullptr};
    QTimer frameTimer{this};
    bool isPaused{false};
};
