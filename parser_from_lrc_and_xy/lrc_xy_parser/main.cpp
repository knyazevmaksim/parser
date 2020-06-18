#include "mainwindow.h"
#include "parser.h"
#include "lrc.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LRC c;
    LRC d;

    Parser &p=c;
    //Parser &g=d;


    MainWindow w(&p,nullptr);
    w.show();

    return a.exec();
}
