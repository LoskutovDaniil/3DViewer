#include "mainwindow.h"
#include "../s21_viewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;  
    w.show();

    return a.exec();
}
