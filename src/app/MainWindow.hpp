#pragma once

#include <QMainWindow>

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    static constexpr int SIMULATION_WIDTH = 800;
    static constexpr int SIMULATION_HEIGHT = 600;
};
