#include "mainwindow.h"

#include <QApplication>
#include "points.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPointSize(9);
    a.setFont(font);

    qRegisterMetaType<Path>();

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    MainWindow w;
    w.show();
    return a.exec();
}
