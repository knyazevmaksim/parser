#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Parser * ptr)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), p_parser{ptr}
{
    ui->setupUi(this);

    connect(ui->label,SIGNAL(sendFileNames(QList<QString>)), p_parser, SLOT(slotGetFileNames(QList<QString>)));
    connect(p_parser,SIGNAL(signalAddText(QByteArray,QColor)), SLOT(slotAddText(QByteArray, QColor)));
    //ui->textBrowser->setStyleSheet()
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotAddText(QByteArray text, QColor color)
{
    //ui->textBrowser->setStyleSheet(QString("color: %1").arg(color.name()));
    //ui->textBrowser->setText(text);
    ui->textBrowser->setTextColor(color);
    ui->textBrowser->insertPlainText(text);
    ui->textBrowser->insertPlainText("\n");
}


