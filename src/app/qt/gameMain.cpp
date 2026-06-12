#include <QApplication>
#include "game/app/qt/mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName(QString::fromUtf8("Dhunanyan"));
    QApplication::setApplicationName(QString::fromUtf8("Blade of Shadows"));
    QApplication::setApplicationDisplayName(QString::fromUtf8("Blade of Shadows"));
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
