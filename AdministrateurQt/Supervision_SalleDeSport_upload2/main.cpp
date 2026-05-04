//main.cpp

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::addLibraryPath(".");
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());

    MainWindow w;
    w.show();
    return a.exec();
}
