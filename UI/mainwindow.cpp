#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    labelList[0] = ui->rearLeft;
    labelList[1] = ui->left1;
    labelList[2] = ui->left2;
    labelList[3] = ui->backLeft;
    labelList[4] = ui->back;
    labelList[5] = ui->backRight;
    labelList[6] = ui->right2;
    labelList[7] = ui->right1;
    labelList[8] = ui->rearRight;
    labelList[9] = ui->rear;

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::ReceiveData);
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();
    }
    delete ui;
}


void MainWindow::on_connectButton_clicked()
{
    if (serial->isOpen()) {
        serial->close();
        ui->connectButton->setText("Connect");
        return;
    }

    serial->setPortName("COM6");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        ui->connectButton->setText("Connected");
    } else {
        ui->label->setText("Failed to connect");
    }
}

void MainWindow::ReceiveData(){
    static QByteArray buffer;

    buffer.append(serial->readAll());

    // Kiểm tra kết thúc chuỗi
    if (buffer.endsWith("\r\n")) {
        QString dataLine = QString::fromUtf8(buffer).trimmed();  // Xoá \r\n
        buffer.clear();

        QStringList values = dataLine.split(',');

        if (values.size() >= 10) {
            for (int i = 0; i < 10; ++i) {
                QString val = values[i].trimmed();
                if (!val.isEmpty()) {
                    labelList[i]->setText(val);
                }
            }
        }
    }
}
