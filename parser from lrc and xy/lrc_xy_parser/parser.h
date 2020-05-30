#ifndef PARSER_H
#define PARSER_H
#include <QWidget>
#include <QFile>
#include <cmath>
#include<QDebug>
#include <map>


enum BlockTypes
{
   SHB=0x0A0D0D0A, //Section Header Block
   EPB=0x00000006,//Enhanced Packet Block
   PB=0x00000002,//Packet Block (Appendix A) считается устаревшим и использоваться не должен
   SPB=0x00000003,//Simple Packet Block
   IDB=0x00000001//Interface Discription Block
};


class Parser: public QWidget
{
    Q_OBJECT
public:
    Parser(QWidget *parent = nullptr);
    void readFile(QString &);
    void analyzeData();



     void testPrint();


private:
    QList<QString> fileNames;
    qint64 charToInt(QByteArray);
    bool littleEndian;
    QByteArray getData(QByteArray);
    int getSourcePort(QByteArray &);
    std::multimap<QByteArray, QByteArray> dictionary;
    QByteArray analyzeMessage(QByteArray &);


    QList<QByteArray> data;


    QString testString {"C:/Users/yhwh/Desktop/LRC.pcapng"};

private slots:
    void slotGetFileNames(QList<QString>);

signals:
    void signalAddText(QByteArray,QColor);

};

#endif // PARSER_H
