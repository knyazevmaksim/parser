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
    emit signalClear();
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
                //зачем это ветвление?
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
//принимает сырые данные с заголовками 3 сетевых слоев: ethernet, ip, tcp
//возвращает данные из первого tcp сегмента из block
//подходит только для использования с ipV4
QByteArray Parser::getData(QByteArray block, int ipVersion)
{
    QByteArray lenIp{""};
    int intLenIp{0};
    if(ipVersion==IPV4)
    {
        lenIp+=block.at(16);
        lenIp+=block.at(17);
        intLenIp=charToInt(lenIp, bigEndian);
        return block.mid(54,intLenIp-40);
    }
    else
    {
        lenIp+=block.at(18);
        lenIp+=block.at(19);
        intLenIp=charToInt(lenIp, bigEndian);
        int ipv6Header=ipv6HeaderLength(block);
        if(ipv6Header!=0)
            return block.mid(14+ipv6Header+20-1, ipv6Header-intLenIp-20);
        else
            return "";
    }

}
//принимает сырые данные с заголовками 3 сетевых слоев: ethernet, ip, tcp
//возвращает порт отправителя сдвинутого на offest байт tcp кадра, используется только для дифференциации отправителей
int Parser::getSourcePort(QByteArray & block, int offset)
{
    QByteArray port={""};
    port.append(block[offset+34]);
    port.append(block[offset+35]);
    int p=charToInt(port, bigEndian);
    return p;
}

//принимает сырые данные с заголовками 3 сетевых слоев: ethernet, ip, tcp
//возвращает порт получателя сдвинутого на offest байт tcp кадра , используется только для дифференциации отправителей
int Parser::getDestinationPort(QByteArray & block, int offset)
{
    QByteArray port={""};
    port.append(block[offset+36]);
    port.append(block[offset+37]);
    int p=charToInt(port, bigEndian);
    return p;
}
//ф-ия check проверяет dataCopy на наличие  LRC протокола и оставляет только подходящие
//ethernet кадры. критерии:инкапсуляция tcp и порта=id=52116
void Parser::check(const int id, QByteArray & data)
{
    QByteArray tmp{""};
    int pos{0};//позиция первого байта проверяемого ethernet кадра
    tmp+=data.at(12);
    tmp+=data.at(13);
    int ipVersion=charToInt(tmp, bigEndian);
    tmp.clear();
    int length{14};
    while(pos!=data.size())
    {
        length=14;
        switch (ipVersion)
        {


        case IPV4:
        {
            tmp+=data.at(pos+16);
            tmp+=data.at(pos+17);
            length+=charToInt(tmp, bigEndian);
            tmp.clear();
            //padding check
            if(length<60/*&&data.mid(length, 60-length).count('\0')==60-length*/)
            {
                data.remove(length,60-length);
            }
            tmp+=data.at(pos+23);

            int nextLayer=charToInt(tmp,bigEndian);
            tmp.clear();
            if(nextLayer==TCP)
            {
                int source{0}, destination{0};
                source=getSourcePort(data, pos);
                destination=getDestinationPort(data, pos);
                if(source==id || destination==id)
                {
                    pos+=length;
                }
                else
                {
                    data.remove(pos,length);
                }
            }
            else
            {
                data.remove(pos,length);
            }
            break;
        }
        case IPV6:
        {
            length+=40;

            tmp+=data.at(pos+18);
            tmp+=data.at(pos+19);
            length+=charToInt(tmp, bigEndian);
            tmp.clear();
            //padding check
            if(length<60/*&&data.mid(length, 60-length).count('\0')==60-length*/)
            {

                data.remove(length,60-length);

            }

            tmp+=data.at(pos+20);//next header

            int nextLayer=charToInt(tmp,bigEndian);
            tmp.clear();
            int i{0};
            if(nextLayer!=TCP)
            {
                int n=data.size();
                //плохой цикл, много ненужных итераций в поисках nextHeader=tcp
                while(nextLayer!=TCP && i<n-pos-54)
                {
                    tmp+=data.at(pos+14+i+40);
                    i+=8;
                    nextLayer=charToInt(tmp,bigEndian);
                    tmp.clear();
                }
                i-=8;
            }
            if(nextLayer==TCP)
            {
                int source{0}, destination{0};
                source=getSourcePort(data, pos+54-34+i);
                destination=getDestinationPort(data, pos+54-36+i);
                if(source==id || destination==id)
                {
                    pos+=length;
                }
                else
                {
                    data.remove(pos,length);
                }
            }
            else
            {
                data.remove(pos,length);
            }
        break;
        }
        case ARP:
        {
            data.remove(pos,60);
            break;
        }
        /*if(ipVersion==IPX)
        {
            не должно быть
        }*/
        default:
            if(ipVersion<=1500)
            {

                data.remove(pos, ipVersion+14);
            }
            else
                data.clear();
        }
    }
}
//возвращает сумму длин расширенного и фиксированного заголовка первого ipv6 инкапсулирующего tcp
//принимает данные с заголовком ethernet
int Parser::ipv6HeaderLength(QByteArray & data)
{
    int length{54};
    QByteArray tmp{""};
    tmp+=data.at(18);
    tmp+=data.at(19);
    length+=charToInt(tmp, bigEndian);
    tmp.clear();
    //padding check
    if(length<60/*&&data.mid(length, 60-length).count('\0')==60-length*/)
    {
        data.remove(length,60-length);
    }
    tmp+=data.at(20);//next header
    int nextLayer=charToInt(tmp,bigEndian);
    int i{0};
    if(nextLayer!=TCP)
    {
        while(nextLayer!=TCP && i<length-40)
        {
            tmp.clear();
            tmp+=data.at(i+40);
            i+=8;
            nextLayer=charToInt(tmp,bigEndian);
        }
        if(i<length-40)
        {
            /*tmp.clear();
            tmp+=data.at(pos+i+40);
            nextLayer=charToInt(tmp,bigEndian);*/

            return 40+i;
        }
        else
            return 0;//tcp не найден
    }
    else
    {
        return 40;
    }


}















