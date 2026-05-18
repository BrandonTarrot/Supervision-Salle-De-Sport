#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    // Force Qt à chercher les drivers SQL dans le bon dossier
    QCoreApplication::addLibraryPath(".");
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());

    MainWindow fenetre;
    fenetre.show();

    return application.exec();
}
