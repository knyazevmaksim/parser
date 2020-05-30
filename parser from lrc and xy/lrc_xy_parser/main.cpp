#include "mainwindow.h"
#include "parser.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QString name{"C:/Users/yhwh/Desktop/LRC.pcapng"};
    Parser p;
    MainWindow w(nullptr, &p);
    //p.readFile(name);

    //p.testPrint();
    w.show();
    //p.analyzeData();
    return a.exec();
}
