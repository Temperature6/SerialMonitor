#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void ScanPort();
    void SendData();
    void SetComPort();
    void ReceiveData();
    void OperatePort();
    void SeeBottom();
    void BaudRate();
    void SaveMessage();
    ~MainWindow();
private:
    QSerialPort* serial;
    QStringList m_serialPortName;
    Ui::MainWindow *ui;
    bool port_ = true;
};

#endif // MAINWINDOW_H
