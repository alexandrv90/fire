#pragma once

#include <QMainWindow>
#include <QPointer>

class FireController;
class FrameMetricsCollector;
class QEvent;
class StatsPanel;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(FireController& fireController,
                        FrameMetricsCollector& frameMetricsCollector,
                        QWidget* parent = nullptr);

protected:
    void changeEvent(QEvent* event) override;

private:
    // Owned by the composition root and guaranteed to outlive this window.
    FireController& fireController;
    // Owned by the widget tree, which may destroy it independently of this window.
    QPointer<StatsPanel> statsPanel;
};
