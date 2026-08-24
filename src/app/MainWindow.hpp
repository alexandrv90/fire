#pragma once

#include <QMainWindow>

class ControlPanel;
class FireController;
class FireWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void presentFrame();
    void updateRunningState(bool running);

private:
    static constexpr int SIMULATION_WIDTH = 800;
    static constexpr int SIMULATION_HEIGHT = 600;

    FireController* fireController{nullptr};
    FireWidget* fireWidget{nullptr};
    ControlPanel* controlPanel{nullptr};
};
