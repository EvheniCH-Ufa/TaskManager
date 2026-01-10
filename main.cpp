#include "mainwindow.h"

#include <QApplication>

// Git test from Qt Creator
// Second test from Qt Creator GUI
// 3 - noteboock

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // УСТАНАВЛИВАЕМ ИМЯ ОРГАНИЗАЦИИ И ПРИЛОЖЕНИЯ
    QCoreApplication::setOrganizationName("EvgenyCHCompany");
    QCoreApplication::setApplicationName("TaskManager");

    MainWindow w;
    w.show();
    return a.exec();
}
