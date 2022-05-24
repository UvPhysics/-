#include "mainwindow.h"
#include <time.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    srand((int)time(0));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
