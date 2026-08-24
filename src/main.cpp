#include "app/MainWindow.hpp"

#include <QApplication>
#include <QtGlobal>

int main(int argc, char* argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication application(argc, argv);
    QApplication::setApplicationDisplayName(QStringLiteral("Classic Fire"));
    QApplication::setOrganizationName(QStringLiteral("Fire Demo"));

    MainWindow window;
    window.show();
    return application.exec();
}
