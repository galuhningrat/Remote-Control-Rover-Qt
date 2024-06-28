#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QLCDNumber>
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void moveForward();
    void moveBackward();
    void turnLeft();
    void turnRight();
    void updateSensorData();
    void readBatteryData();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QSerialPort *batterySerial;
    QTimer *batteryTimer;
};

#endif // MAINWINDOW_H
