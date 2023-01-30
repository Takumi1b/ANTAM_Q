#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(QString("ANTAM Q"));
    MainWindow w;
    w.setWindowTitle(QString("ANTAM Q"));
    w.show();
    return a.exec();
}
