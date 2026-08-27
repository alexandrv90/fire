#include "app/FireController.hpp"
#include "app/FrameMetricsCollector.hpp"
#include "app/MainWindow.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QtGlobal>

int main(int argc, char* argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication application(argc, argv);
    QApplication::setApplicationDisplayName(QStringLiteral("Fire Demo"));
    QApplication::setOrganizationName(QStringLiteral("Fire Demo"));

    // Hot path should never throw, construction errors caught here
    try {
        // Both are borrowed by widgets inside the window, so they must outlive it.
        // The controller additionally owns the zero-copy frame storage FireView aliases.
        FrameMetricsCollector frameMetricsCollector;
        FireController fireController;
        MainWindow window{fireController, frameMetricsCollector};
        window.show();
        return application.exec();
    } catch (const std::exception& error) {
        QMessageBox::critical(nullptr,
                              QStringLiteral("Fire Demo"),
                              QStringLiteral("Failed to start: %1").arg(QString::fromUtf8(error.what())));
        return EXIT_FAILURE;
    }
}
