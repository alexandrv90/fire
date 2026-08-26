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
        MainWindow window;
        window.show();
        return application.exec();
    } catch (const std::exception& error) {
        QMessageBox::critical(nullptr,
                              QStringLiteral("Fire Demo"),
                              QStringLiteral("Failed to start: %1").arg(QString::fromUtf8(error.what())));
        return EXIT_FAILURE;
    }
}
