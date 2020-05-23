#include "mainwindow.h"

#include <QApplication>
#include "points.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<Path>();

    MainWindow w;
    w.show();
    return a.exec();
}
