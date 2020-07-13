#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Parser * ptr, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), p_parser{ptr}
{
    ui->setupUi(this);

    connect(ui->label,SIGNAL(sendFileNames(QList<QString>)), p_parser, SLOT(slotGetFileNames(QList<QString>)));
    connect(p_parser,SIGNAL(signalAddText(QByteArray,QColor)), SLOT(slotAddText(QByteArray, QColor)));
    connect(p_parser,SIGNAL(signalAddText(QString,QColor)), SLOT(slotAddText(QString, QColor)));
    connect(ui->pushButton, SIGNAL(clicked()),SLOT(slotBrowseFiles()));
    connect(this, SIGNAL(sendFileNames(QList<QString>)), p_parser, SLOT(slotGetFileNames(QList<QString>)));
    connect(p_parser,SIGNAL(signalClear()), SLOT(slotClear()));
}

void MainWindow::slotClear()
{
    ui->textBrowser->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotAddText(QByteArray text, QColor color)
{
    ui->textBrowser->setTextColor(color);
    ui->textBrowser->insertPlainText(text);
    ui->textBrowser->insertPlainText("\n");
}

void MainWindow::slotAddText(QString text, QColor color)
{
    ui->textBrowser->setTextColor(color);
    ui->textBrowser->insertPlainText(text);
    ui->textBrowser->insertPlainText("\n");
}

void MainWindow::slotBrowseFiles()
{
    QList<QString> d=QFileDialog::getOpenFileNames(0,"Open dialog","", "*.pcapng");
    emit sendFileNames(d);
}
