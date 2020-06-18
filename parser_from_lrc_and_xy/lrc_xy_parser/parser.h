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
enum byteOrders
{
    littleEndian=true,//от младшего байта к старшему слева направо
    bigEndian=false //от старшего байта к младшему слева направо
};


class Parser: public QWidget
{
    Q_OBJECT
public:
    Parser(QWidget *parent = nullptr);
    virtual ~Parser();

    void readFile(QString &);//!

    virtual void analyzeData()=0;//!


protected:
    QList<QByteArray> data;//!
    QList<QString> fileNames;//!


    qint64 charToInt(QByteArray, bool);//!
    QByteArray getData(QByteArray);//оставить как есть оставить как есть если XY использует TCP
    int getSourcePort(QByteArray &);//оставить как есть если XY использует TCP

    virtual const std::multimap<QByteArray, QByteArray> & create_dictionary()=0;//!
    virtual QByteArray analyzeMessage(QByteArray &)=0;//!

private slots:
    void slotGetFileNames(QList<QString>);//!

signals:
    void signalAddText(QByteArray,QColor);//!
    void signalAddText(QString,QColor);//!

};

#endif // PARSER_H
