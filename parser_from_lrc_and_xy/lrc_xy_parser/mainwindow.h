#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QKeyEvent>
#include <QDebug>
#include<QFileDialog>
#include "drop.h"
#include "parser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Parser *ptr,QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    Parser * p_parser;

private slots:
    void slotAddText(QByteArray, QColor);
    void slotAddText(QString, QColor);
    void slotBrowseFiles();
    void slotClear();

signals:
    void sendFileNames(QList<QString>);

};
#endif // MAINWINDOW_H
