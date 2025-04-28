#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void ReceiveData();

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer *timer;
    QString logData;
    QLabel *labelList[10];
};
#endif // MAINWINDOW_H
