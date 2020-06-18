#include "lrc.h"

LRC::LRC(QWidget * parent):Parser{parent}
{
    qDebug()<<"lrc";
}

LRC::~LRC()
{

}

//разбиваем данные на сообщения LRC для анализа и анализируем их
void LRC::analyzeData()
{
    QColor green(0,204,0), blue(24,38,176);
    QColor* color{&blue};
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
                *cur=cur->mid(1,cur->size()-2);
                emit signalAddText(analyzeMessage(*cur),*color);
                tmp.remove(startPos,endPos+1);
                cur->clear();
            }
        }
        prevPort=curPort;
    }
}


const std::multimap<QByteArray, QByteArray> & LRC::create_dictionary()
{
    static const std::multimap<QByteArray, QByteArray> dic=
    {{"XPOINT","Crosspoint"},{"LOCK","Lock destination"},
     {"PROTECT","Protect destination"},{"CHANNELS","Channel information"},
     {"DEST","Destination information"},{"SRC","Source information"},
     {"DBCHANGE","Database update"},{"CONNECTION","Reporting mode"},
     {"PROTOCOL","Protocol information"},{"XBUFFER","Crosspoint buffer"},
     {"XDISCONNECT","Disconnect destination"},{"XPRESET","Preset command"},
     {"XSALVO","Crosspoint salvo"},{"XPOINTERR","Crosspoint error "},
     {":","request to change"},{"!","report status change"},
     {"?","query current status"},{"%","report current status"},
     {";","and"},{"","breakaway/all "},{"D","destination "}, {"S","to source "},
     {"U","by user "},{"V","value is "},{"O","override lock status"}, {"I","channel id list"},
     {"NAME","name "}, {"Q","query"}, {"ID","id "},{"CHANNELS","valid channels"},{"COUNT","count "},
     {"PHYSICAL","physical location of a channel"},{"LOCATION","physical location of a slot"},
     {"DATA","type of change "},{"MODE","reporting mode "},
     {"VERSION","protocol version "},{"F","flags "},{"E","error is "}};
    return dic;
}

//на вход 1 LRC сообщение без символа начала и символа конца
QByteArray LRC::analyzeMessage(QByteArray & message)
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
