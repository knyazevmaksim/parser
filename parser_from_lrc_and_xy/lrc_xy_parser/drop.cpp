#include "drop.h"

Drop::Drop(QWidget *parent) : QLabel("Drop area",parent)
{
    setAcceptDrops(true);
    this->setPixmap(QPixmap(":images/resources/dragndrop.png"));
    setToolTip("Drag&Drop area");
}

void Drop::dragEnterEvent(QDragEnterEvent * event)
{
    event->acceptProposedAction();
}

void Drop::dragLeaveEvent(QDragLeaveEvent * event)
{

    this->setPixmap(QPixmap(":images/resources/dragndrop.png"));
}


void Drop::dropEvent(QDropEvent * event)
{
    QList<QUrl> urlList=event->mimeData()->urls();
    QList<QString> names;
    QString str{""};
    foreach(QUrl url, urlList)
    {
        str+=url.toLocalFile()+"\n";
        names.push_back(url.toLocalFile());
    }
    setText("Dropped:\n"+str);
    //передача имен файлов в парсер
    emit sendFileNames(names);
}

void Drop::dragMoveEvent(QDragMoveEvent * event)
{
    bool flag{true};
    QString str{""};
    foreach(QUrl url, event->mimeData()->urls())
    {
        str+=url.toLocalFile()+"\n";
        if(!str.contains(".pcapng"))
        {   flag=false;
            break;
        }
    }
    if (flag && event->mimeData()->hasFormat("text/uri-list"))
    {
        this->setPixmap(QPixmap (":images/resources/drag.png"));
    }
    else
    {
        this->setPixmap(QPixmap (":images/resources/no.png"));
        event->ignore();
    }
}
