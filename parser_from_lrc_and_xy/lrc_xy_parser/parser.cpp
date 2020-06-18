#include "parser.h"

Parser::Parser(QWidget *parent) : QWidget{parent}
{
    qDebug()<<"parser";
}

Parser::~Parser()
{

}

//
void Parser::slotGetFileNames(QList<QString> a)
{
    QColor black(0,0,0);
    QString tmp{""};
    fileNames<<a;//do we need to store filenames? its not necessary yet
    foreach(QString name, fileNames)
    {
        readFile(name);
        tmp=name.append(" :");
        emit signalAddText(name,black);
        analyzeData();
        data.clear();
    }
    fileNames.clear();//
}
//считывание данных из файла. сохраняет в список все данные включая заголовки всех протоколов
void Parser::readFile(QString & fileName)
{
    bool byteOrder{bigEndian};
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray str{""};
    qint64 position{0};
    qint64 blockType{0};
    qint64 blockSize{0};
    int dataSize{0};
    while(!file.atEnd())
    {
        file.seek(position);
        str=file.read(4);
        blockType=charToInt(str,byteOrder);
        //рассмотрим 3 случая:
        //либо это блок SPB либо EPB , либо SHB  либо нет и идем дальше
        switch(blockType)
        {
            case SHB:
                file.seek(8);
                str=file.read(4);
                if(0x1A2B3C4D==charToInt(str,byteOrder))
                    byteOrder=bigEndian;
                else
                    byteOrder=littleEndian;
                file.seek(4);
                str=file.read(4);
                blockSize=charToInt(str,byteOrder);
                position+=blockSize;
            break;
            case SPB:
            //в этом блоке нет информации о длине дата поля, только о длине пакета передаваемого по сети
            //можно сравнить длину поля original packet length c block total length-16 байт
            //в случае его "обреза" (непонятно что делать, потому что каждый блок может содержать порцию захваченного пакета)
                file.seek(position+4);
                str=file.read(4);
                blockSize=charToInt(str,byteOrder);
                dataSize=blockSize-16;
                file.seek(position+8);
                str=file.read(4);
                if(dataSize==charToInt(str,byteOrder))
                {
                //читаем данные
                    file.seek(position+8);
                    str=file.read(dataSize);
                    data.push_back(str);
                    position+=blockSize;
                }
                else
                {
                //либо пакет был обрезан либо разделен на несколько блоков
                    file.seek(position+8);
                    str=file.read(dataSize);
                    data.push_back(str);
                    position+=blockSize;
                    qDebug()<<"packet has been truncated or divided into several blocks"<<endl;
                }
            break;
            case EPB:
                str=file.read(4);
                blockSize=charToInt(str,byteOrder);
                file.seek(position+20);
                str=file.read(4);
                dataSize=charToInt(str,byteOrder);
                file.seek(position+28);
                str=file.read(dataSize);
                data.push_back(str);
                position+=blockSize;
            break;
            /*case IDB:
                qDebug()<<i;
                i++;
                file.seek(position+4);
                str=file.read(4);
                blockSize=charToInt(str,byteOrder);
                position+=blockSize;
            break;*/
            default:
                file.seek(position+4);
                str=file.read(4);
                blockSize=charToInt(str,byteOrder);
                position+=blockSize;
            break;
        }
    }
    file.close();
}
//функция переводит массив символов char в шестнадцатиричной с.с. в десятичное число согласно представлению little endian
//или big endian соответствующего отрицанию little endian
qint64 Parser::charToInt(QByteArray arr, bool byteOrder)
{
    qint64 j{0}, tmp{0}, fValue{0}, sValue{0};
    if (byteOrder==bigEndian)
        for (int i=0; i<arr.size(); i++)
        {
            tmp=arr[i];
            if(tmp<0)
                tmp=128+(128+tmp);
            fValue=tmp/16;
            sValue=tmp-fValue*16;
            j+=fValue*pow(16, 2*arr.size()-2*i-1);
            j+=sValue*pow(16, 2*arr.size()-2*i-2);
        }
    else
        for (int i=arr.size()-1; i>=0; i--)
        {
            tmp=arr[i];
            if(tmp<0)
                tmp=128+(128+tmp);
            fValue=tmp/16;
            sValue=tmp-fValue*16;
            j+=fValue*pow(16, 2*i+1);
            j+=sValue*pow(16, 2*i);
        }
    return j;
}
//функция получает данные из tcp сегмента
QByteArray Parser::getData(QByteArray block)
{
    QByteArray lenIp{""};
    lenIp+=block.at(16);
    lenIp+=block.at(17);
    int intLenIp=charToInt(lenIp, bigEndian);
    return block.mid(54,intLenIp-40);
}
//получает порт отправителя tcp кадра, используется только для дифференциации отправителей
int Parser::getSourcePort(QByteArray & block)
{
    QByteArray port={""};
    port.append(block[34]);
    port.append(block[35]);
    int p=charToInt(port, bigEndian);
    return p;
}















