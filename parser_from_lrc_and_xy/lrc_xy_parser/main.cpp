#include "mainwindow.h"
#include "parser.h"
#include "lrc.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LRC c;

    Parser &p=c;

    MainWindow w(&p,nullptr);
    w.show();

    return a.exec();
}
