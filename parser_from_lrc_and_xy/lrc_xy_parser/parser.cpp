#include "parser.h"

Parser::Parser(QWidget *parent) : QWidget{parent}, littleEndian{false}
{
    dictionary.insert(std::pair<QByteArray, QByteArray>("XPOINT","Crosspoint"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("LOCK","Lock destination"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("PROTECT","Protect destination"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("CHANNELS","Channel information"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("DEST","Destination information"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("SRC","Source information"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("DBCHANGE","Database update"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("CONNECTION","Reporting mode"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("PROTOCOL","Protocol information"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("XBUFFER","Crosspoint buffer"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("XDISCONNECT","Disconnect destination"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("XPRESET","Preset command"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("XSALVO","Crosspoint salvo"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("XPOINTERR","Crosspoint error "));
    dictionary.insert(std::pair<QByteArray, QByteArray>(":","request to change"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("!","report status change"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("?","query current status"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("%","report current status"));
    dictionary.insert(std::pair<QByteArray, QByteArray>(";","and"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("","breakaway"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("","all"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("D","destination "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("S","to source "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("U","by user "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("V","value is "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("O","override lock status"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("I","channel id list"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("NAME","name "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("Q","query"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("ID","id "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("CHANNELS","valid channels"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("COUNT","count "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("PHYSICAL","physical location of a channel"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("LOCATION","physical location of a slot"));
    dictionary.insert(std::pair<QByteArray, QByteArray>("DATA","type of change "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("MODE","reporting mode "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("VERSION","protocol version "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("F","flags "));
    dictionary.insert(std::pair<QByteArray, QByteArray>("E","erros is "));

}
//на вход 1 LRC сообщение без символа начала и символа конца
QByteArray Parser::analyzeMessage(QByteArray & message)
{
    QByteArray sentence{""};
    QByteArray lexeme{""};
    //int pos=message.indexOf(':');
    int pos{0};

    while(message.at(pos)!=':'&& message.at(pos)!='!'&& message.at(pos)!='?'&& message.at(pos)!='%'&&pos<message.size())
    {
        lexeme+=message.at(pos);
        pos++;
    }
    auto it=dictionary.find(lexeme);
    if(it!=dictionary.end())
    {
        sentence+=it->second;
        sentence+=' ';
        //рассмотреть случай с повторяющимися значениями ключа в ммапе
        lexeme.clear();
    }
    else
        return message;
    lexeme+=message.at(pos);//получение оператора
    pos++;
    it=dictionary.find(lexeme);
    sentence+=it->second;
    sentence+=' ';
    lexeme.clear();
    //sentence+=message.mid(pos+1,message.size()-1-pos);
    while(pos!=message.size())
    {
        if(message.at(pos)==';')
        {
            sentence+=" ";
            pos++;
        }
        while(message.at(pos)!='$'&& message.at(pos)!='#'&& message.at(pos)!='&'&& pos<message.size())
        {
            lexeme+=message.at(pos);
            pos++;
        }
        it=dictionary.find(lexeme);
        sentence+=it->second;
        sentence+=' ';
        //рассмотреть случай с повторяющимися значениями ключа в ммапе
        lexeme.clear();
        pos+=2;
        while(message.at(pos)!='}'&& pos<message.size())
        {
            lexeme+=message.at(pos);
            pos++;
        }
        sentence+=lexeme;
        sentence+=' ';
        //рассмотреть случай с повторяющимися значениями ключа в ммапе
        lexeme.clear();
        pos++;
    }
return sentence;
}
//разбиваем данные на сообщения LRC для анализа и пишем пока что в тестовый файл
void Parser::analyzeData()
{
    QColor green(0,204,0), blue(24,38,176);
    QColor* color{&blue};
    QFile file("C:/Users/yhwh/Desktop/test2.txt");
    file.open(QIODevice::WriteOnly);
    int startPos{0}, endPos{0};
    QByteArray tmp{""}, firstEnd{""}, secondEnd{""};//2 порта общаются
    QByteArray * cur{&firstEnd};
    int prevPort{0}, curPort{0};
    foreach(QByteArray arr, data)
    {
        tmp=getData(arr);
        curPort=getSourcePort(arr);
        if(curPort!=prevPort)
        {
            //std::swap(cur,prev);
            if(cur==&firstEnd)
                cur=&secondEnd;
            else
                cur=&firstEnd;
            if(color==&green)
                color=&blue;
            else
                color=&green;
        }
        while(!tmp.isEmpty())
        {
            startPos=tmp.indexOf('~');
            endPos=tmp.indexOf('\\');
            if(startPos==-1 && endPos==-1)
            {
                if(cur->contains('~'))
                {
                    *cur+=tmp;

                }
                tmp.clear();
            }
            if(startPos!=-1 && endPos==-1)
            {
                if(cur->isEmpty())
                    *cur+=tmp.mid(startPos,tmp.size()-startPos);
                tmp.clear();
            }
            if(startPos==-1 && endPos!=-1)
            {
                //здесь остаток дополняется до конца сообщения
                if(cur->contains('~'))
                {
                    *cur+=tmp.mid(0,endPos+1);
                    file.write(*cur);
                    file.write("\r\n");
                    *cur=cur->mid(1,cur->size()-2);
                    emit signalAddText(analyzeMessage(*cur),*color);
                }
                tmp.clear();
                cur->clear();
            }
            if(startPos!=-1 && endPos!=-1 && endPos<startPos)
            {
                if(cur->contains('~'))
                {
                    *cur+=tmp.mid(0,endPos+1);
                    file.write(*cur);
                    file.write("\r\n");
                    *cur=cur->mid(1,cur->size()-2);
                    emit signalAddText(analyzeMessage(*cur),*color);
                }
                tmp.remove(0,endPos+1);
                cur->clear();
            }
            if(startPos!=-1 && endPos!=-1 && endPos>startPos)
            {
                //только в этом варианте сразу получаем целое сообщение
                if(cur->isEmpty())
                {
                    cur->clear();
                }
                *cur+=tmp.mid(startPos,endPos+1);
                file.write(*cur);
                file.write("\r\n");
                *cur=cur->mid(1,cur->size()-2);
                emit signalAddText(analyzeMessage(*cur),*color);
                tmp.remove(startPos,endPos+1);
                cur->clear();
            }
        }
        prevPort=curPort;
    }
}

void Parser::slotGetFileNames(QList<QString> a)
{
    //при добавлении новых файлов пути старых файлов остаются
    fileNames<<a;
    readFile(fileNames[0]);
    analyzeData();
}

void Parser::readFile(QString & fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray str{""};
    qint64 position{0};
    qint64 blockType{0};
    qint64 blockSize{0};
    int dataSize{0};
    //int i{0};
    while(!file.atEnd())
    {
        file.seek(position);
        str=file.read(4);
        blockType=charToInt(str);
        //qDebug()<<blockType<<endl;
        //рассмотрим 3 случая:
        //либо это блок SPB либо EPB , либо SHB  либо нет и идем дальше
        switch(blockType)
        {
            case SHB:
                file.seek(8);
                str=file.read(4);
                if(0x1A2B3C4D==charToInt(str))
                    littleEndian=false;
                else
                    littleEndian=true;
                file.seek(4);
                str=file.read(4);
                blockSize=charToInt(str);
                position+=blockSize;
            break;
            case SPB:
            //в этом блоке нет информации о длине дата поля, только о длине пакета передаваемого по сети
            //можно сравнить длину поля original packet length c block total length-16 байт
            //в случае его "обреза" (непонятно что делать, потому что каждый блок может содержать порцию захваченного пакета)
                file.seek(position+4);
                str=file.read(4);
                blockSize=charToInt(str);
                dataSize=blockSize-16;
                file.seek(position+8);
                str=file.read(4);
                if(dataSize==charToInt(str))
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
                blockSize=charToInt(str);
                file.seek(position+20);
                str=file.read(4);
                dataSize=charToInt(str);
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
                blockSize=charToInt(str);
                position+=blockSize;
            break;*/
            default:
                file.seek(position+4);
                str=file.read(4);
                blockSize=charToInt(str);
                position+=blockSize;
            break;
        }
    }
    file.close();
}

//может быть не будет хватать диапозона int
qint64 Parser::charToInt(QByteArray arr)
{
    qint64 j{0}, tmp{0}, fValue{0}, sValue{0};
    if (!littleEndian)
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

void Parser::testPrint()
{
    QFile file("C:/Users/yhwh/Desktop/test1.txt");
    file.open(QIODevice::WriteOnly);
    QByteArray p{""};
    int port{0};
    foreach(QByteArray arr, data)
    {
        port=getSourcePort(arr);
        p=QByteArray::number(port);
        p.prepend("source:");
        file.write(p);
        file.write("\n");
        file.write(getData(arr));
    }
    file.write("\n");
    file.write("\n");
    p="XPOINT%D${Ds 1.Level 38};S${Vr 9.Level 38}";
    file.write(analyzeMessage(p));
    file.close();
}

QByteArray Parser::getData(QByteArray block)
{
    QByteArray lenIp{""};
    lenIp+=block.at(16);
    lenIp+=block.at(17);
    int intLenIp=charToInt(lenIp);
    return block.mid(54,intLenIp-40);
}

int Parser::getSourcePort(QByteArray & block)
{
    QByteArray port={""};
    littleEndian=false;
    port.append(block[34]);
    port.append(block[35]);
    int p=charToInt(port);
    //littleEndian=true;
    return p;
}















