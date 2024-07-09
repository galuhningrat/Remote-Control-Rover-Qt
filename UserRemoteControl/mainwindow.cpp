#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , batterySerial(new QSerialPort(this))
    , batteryTimer(new QTimer(this))
    , maxExpectedPower(6.0)
    , batteryDataBuffer()
{
    ui->setupUi(this);

    // Connect signals and slots
    connect(ui->forwardButton, &QPushButton::pressed, this, &MainWindow::moveForward);
    connect(ui->backwardButton, &QPushButton::pressed, this, &MainWindow::moveBackward);
    connect(ui->leftButton, &QPushButton::pressed, this, &MainWindow::turnLeft);
    connect(ui->rightButton, &QPushButton::pressed, this, &MainWindow::turnRight);

    // Setup serial port
    serial = new QSerialPort(this);
    serial->setPortName("COM3"); // Adjust to your port
    serial->setBaudRate(QSerialPort::Baud9600);
    if (serial->open(QIODevice::ReadWrite)) {
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::updateSensorData);
    } else {
        qDebug() << "Failed to open main serial port.";
    }

    // Setup progress bar
    powerProgressBar = ui->persentase;
    powerProgressBar->setRange(0, 100);
    powerProgressBar->setValue(0);
    powerProgressBar->setTextVisible(true);
    powerProgressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #05B8CC;"
        "   width: 20px;"
        "}"
        );

    // Setup battery serial port
    batterySerial->setPortName("COM10"); // Adjust to your port
    batterySerial->setBaudRate(QSerialPort::Baud115200);
    batterySerial->setDataBits(QSerialPort::Data8);
    batterySerial->setParity(QSerialPort::NoParity);
    batterySerial->setStopBits(QSerialPort::OneStop);
    batterySerial->setFlowControl(QSerialPort::NoFlowControl);

    if (batterySerial->open(QIODevice::ReadOnly)) {
        connect(batterySerial, &QSerialPort::readyRead, this, &MainWindow::readBatteryData);
        qDebug() << "Battery serial port opened successfully.";
    } else {
        qDebug() << "Failed to open battery serial port.";
    }

    batteryTimer->start(2000);  // Update battery data every 2 seconds
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveForward() {
    serial->write("FORWARD\n");
}

void MainWindow::moveBackward() {
    serial->write("BACKWARD\n");
}

void MainWindow::turnLeft() {
    serial->write("LEFT\n");
}

void MainWindow::turnRight() {
    serial->write("RIGHT\n");
}

void MainWindow::updateSensorData() {
    QByteArray data = serial->readAll();
    QList<QByteArray> sensorData = data.split(',');

    if (sensorData.size() >= 6) {
        ui->cameraLabel->setText(sensorData.at(0));
        ui->gpsLabel->setText(sensorData.at(2));
        ui->accelerometerLabel->setText(sensorData.at(3));
        ui->imuLabel->setText(sensorData.at(4));
        ui->ultrasonicLabel->setText(sensorData.at(5));
        ui->speedLcd->display(sensorData.at(6).toInt());
    }
}

void MainWindow::readBatteryData() {
    batteryDataBuffer.append(batterySerial->readAll());

    while (batteryDataBuffer.contains('\n')) {
        int newlineIndex = batteryDataBuffer.indexOf('\n');
        QString dataString = batteryDataBuffer.left(newlineIndex).trimmed();
        batteryDataBuffer.remove(0, newlineIndex + 1);

        QStringList values = dataString.split(',');
        if (values.size() >= 3) {
            float power = values.at(2).toFloat();
            int percentage = qRound((power / maxExpectedPower) * 100);
            percentage = qMin(percentage, 100);
            powerProgressBar->setValue(percentage);
            powerProgressBar->setFormat(QString("%1% (%2W / %3W)").arg(percentage).arg(power, 0, 'f', 1).arg(maxExpectedPower, 0, 'f', 1));
        } else {
            qDebug() << "Incomplete battery data received: " << dataString;
        }
    }
}
