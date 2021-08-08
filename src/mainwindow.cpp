#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QStringList>
#include <QSerialPortInfo>
#include <QDebug>
#include <QByteArray>
#include <QDateTime>
#include <QFileDialog>
#include <QStandardPaths>
#include <QUrl>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //窗口初始化
    serial = new QSerialPort;
    ScanPort();
    SetComPort();
    ui->statusBar->setStyleSheet("QStatusBar{background-color:#3a9aeb;}");
    int baud_val[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 74800, 115200};
    for (int i = 0; i < 9; i++)
    {
        ui->BaudRate->addItem(QString::number(baud_val[i]));
    }
    ui->BaudRate->setCurrentText("9600");
    ui->operate_port->setText("关闭串口");
    ui->operate_port->setIcon(QIcon(":/pic/port_close.png"));
    //串口配置初始化
    serial->setBaudRate(ui->BaudRate->currentText().toInt(),QSerialPort::AllDirections);//设置波特率和读写方向
    serial->setDataBits(QSerialPort::Data8);		//数据位为8位
    serial->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    serial->setParity(QSerialPort::NoParity);	//无校验位
    serial->setStopBits(QSerialPort::OneStop); //一位停止位
    //链接槽和信号
    connect(ui->refrush, &QPushButton::clicked, this, &MainWindow::ScanPort);
    connect(ui->Send_btn, &QPushButton::clicked, this, &MainWindow::SendData);
    connect(ui->port_list, &QComboBox::currentTextChanged, this, &MainWindow::SetComPort);
    connect(serial,&QSerialPort::readyRead,this,&MainWindow::ReceiveData);
    connect(ui->operate_port, &QAction::triggered, this, &MainWindow::OperatePort);
    connect(ui->msg, &QTextBrowser::textChanged, this, &MainWindow::SeeBottom);
    connect(ui->BaudRate, &QComboBox::currentTextChanged, this, &MainWindow::BaudRate);
    connect(ui->save_msg, &QAction::triggered, this, &MainWindow::SaveMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ScanPort()
{
    ui->port_list->clear();
    m_serialPortName.clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
    }
    ui->port_list->addItems(m_serialPortName);
}

void MainWindow::SendData()
{
    serial->write(ui->message->text().toLatin1().data());
    ui->message->clear();
    return;
}

void MainWindow::SetComPort()
{
    serial->close();
    serial->setPortName(ui->port_list->currentText());
    ui->statusBar->showMessage("当前串口为:" + ui->port_list->currentText());
    ui->operate_port->setIcon(QIcon(":/pic/port_close"));
    ui->operate_port->setText("关闭串口");
    port_ = true;
    serial->open(QIODevice::ReadWrite);
    return;
}

void MainWindow::ReceiveData()
{
    QString data_str;
    if (ui->isShowTime->isChecked())
    {
        data_str = QDateTime::currentDateTime().toString("|hh:mm:ss.zzz") + " -> ";
    }
    data_str += (QString)serial->readAll();
    ui->msg->insertPlainText(data_str);
}

void MainWindow::OperatePort()
{
    if (port_)
    {
        //串口开着，关闭它
        serial->close();
        ui->operate_port->setText("打开串口");
        ui->operate_port->setIcon(QIcon(":/pic/port_open"));
        port_ = false;
        ui->statusBar->showMessage("串口 " + ui->port_list->currentText() + " 已关闭");
    }
    else
    {
        serial->open(QIODevice::ReadWrite);
        ui->operate_port->setText("关闭串口");
        ui->operate_port->setIcon(QIcon(":/pic/port_close"));
        port_ = true;
        ui->statusBar->showMessage("当前串口为:" + ui->port_list->currentText());
    }
    return;
}

void MainWindow::SeeBottom()
{
    ui->msg->moveCursor(QTextCursor::End);
}

void MainWindow::BaudRate()
{
    serial->setBaudRate(ui->BaudRate->currentText().toInt(),QSerialPort::AllDirections);
}

void MainWindow::SaveMessage()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存文件", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "文本文档(*.txt)");
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return;
    }
    else
    {
        QTextStream stream(&file);
        stream << ui->msg->toPlainText();
        stream.flush();
        file.close();
        ui->statusBar->showMessage("文件保存到 "+ filename, 2000);
    }
}
