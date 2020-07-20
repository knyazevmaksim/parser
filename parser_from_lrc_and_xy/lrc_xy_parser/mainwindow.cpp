#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Parser * ptr, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), p_parser{ptr}
{
    ui->setupUi(this);
    this->setMinimumSize(400,400);
    ui->textBrowser->setMinimumSize(175,150);
    ui->pushButton->setMaximumHeight(50);
    ui->label->setMaximumSize(350,350);
    ui->label->setMinimumSize(100,85);
    QWidget * wid=new QWidget;
    QVBoxLayout * p_vbxLayout=new QVBoxLayout;
    QHBoxLayout * p_hbxLayout=new QHBoxLayout;

    p_hbxLayout->addWidget(ui->label);
    p_hbxLayout->addWidget(ui->textBrowser);
    p_hbxLayout->setContentsMargins(5, 5, 5, 5);
    p_hbxLayout->setSpacing(20);

    p_vbxLayout->addLayout(p_hbxLayout);
    p_vbxLayout->addWidget(ui->pushButton);
    p_hbxLayout->setContentsMargins(5, 5, 5, 5);
    p_hbxLayout->setSpacing(20);
    wid->setLayout(p_vbxLayout);
    this->setCentralWidget(wid);

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    //ui->label->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    //ui->pushButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    //ui->textBrowser->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    this->setWindowTitle("Parser");
    this->setWindowIcon(QPixmap(":/images/main_icon.png"));
    p_pb=new QProgressBar;
    p_pb->setRange(0,100);
    p_pb->setMinimumWidth(400);
    p_pb->setAlignment(Qt::AlignCenter);
    p_pb->setWindowTitle("Progress:");
    p_pb->setValue(0);
    connect(ui->label,SIGNAL(sendFileNames(QList<QString>)), p_parser, SLOT(slotGetFileNames(QList<QString>)));
    connect(p_parser,SIGNAL(signalAddText(QByteArray,QColor)), SLOT(slotAddText(QByteArray, QColor)));
    connect(p_parser,SIGNAL(signalAddText(QString,QColor)), SLOT(slotAddText(QString, QColor)));
    connect(ui->pushButton, SIGNAL(clicked()),SLOT(slotBrowseFiles()));
    connect(this, SIGNAL(sendFileNames(QList<QString>)), p_parser, SLOT(slotGetFileNames(QList<QString>)));
    connect(p_parser,SIGNAL(signalClear()), SLOT(slotClear()));
    connect(p_parser,SIGNAL(signalShowProgressBar(QString)), SLOT(slotShowProgressBar(QString)));
    connect(p_parser,SIGNAL(signalSetProgressValue(int)), SLOT(slotSetProgressValue(int)));
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

void MainWindow::slotShowProgressBar(QString str)
{
    p_pb->setWindowTitle(str);
    p_pb->setValue(0);
    p_pb->show();
}

void MainWindow::slotSetProgressValue(int value)
{
    p_pb->setValue(value);
    if (p_pb->value()==100)
    {
        p_pb->hide();
    }
}
