#pragma once

#include <QMainWindow>

class FireController;
class QEvent;
class StatsPanel;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void changeEvent(QEvent* event) override;

private:
    FireController* fireController{nullptr};
    StatsPanel* statsPanel{nullptr};
};
