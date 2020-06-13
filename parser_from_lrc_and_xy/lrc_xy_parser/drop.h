#ifndef DROP_H
#define DROP_H
#include <QLabel>
#include <QtWidgets>
#include<QDragEnterEvent>

class Drop : public QLabel
{
    Q_OBJECT
public:
    explicit Drop(QWidget *parent = nullptr);

private:

protected:
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dropEvent(QDropEvent *);

signals:
    void sendFileNames(QList<QString>);

};

#endif // DROP_H
