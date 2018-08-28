#include "mainwindow.h"
#include <QApplication>
#include <mongoc.h>

int main(int argc, char *argv[])
{
    int res;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // init mongo c driver
    mongoc_init();
    // launch applications
    res = a.exec();
    mongoc_cleanup ();
    return res;
}