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
    virtual void dragEnterEvent(QDragEnterEvent *) override;
    virtual void dropEvent(QDropEvent *) override;
    virtual void dragLeaveEvent(QDragLeaveEvent *) override;
    virtual void dragMoveEvent(QDragMoveEvent *) override;
signals:
    void sendFileNames(QList<QString>);

};

#endif // DROP_H
