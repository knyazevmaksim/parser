#include "drop.h"

Drop::Drop(QWidget *parent) : QLabel("Drop area",parent)
{
    setAcceptDrops(true);
}

void Drop::dragEnterEvent(QDragEnterEvent * event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void Drop::dropEvent(QDropEvent * event)
{
    QList<QUrl> urlList=event->mimeData()->urls();
    QList<QString> names;
    QString str{""};
    foreach(QUrl url, urlList)
    {
        str+=url.toString()+"\n";
        names.push_back(url.toLocalFile());
    }
    setText("Dropped:\n"+str);

    //передача имен файлов в парсер
    emit sendFileNames(names);
}
