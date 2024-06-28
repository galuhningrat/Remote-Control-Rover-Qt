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
{
    ui->setupUi(this);

    // Menghubungkan sinyal dan slot
    connect(ui->forwardButton, &QPushButton::pressed, this, &MainWindow::moveForward);
    connect(ui->backwardButton, &QPushButton::pressed, this, &MainWindow::moveBackward);
    connect(ui->leftButton, &QPushButton::pressed, this, &MainWindow::turnLeft);
    connect(ui->rightButton, &QPushButton::pressed, this, &MainWindow::turnRight);

    // Setup serial port
    serial = new QSerialPort(this);
    serial->setPortName("COM3"); // Sesuaikan dengan port yang Anda gunakan
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->open(QIODevice::ReadWrite);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::updateSensorData);

    // Setup battery serial port
    batterySerial->setPortName("COM10"); // Sesuaikan dengan port yang Anda gunakan
    batterySerial->setBaudRate(QSerialPort::Baud115200);
    batterySerial->setDataBits(QSerialPort::Data8);
    batterySerial->setParity(QSerialPort::NoParity);
    batterySerial->setStopBits(QSerialPort::OneStop);
    batterySerial->setFlowControl(QSerialPort::NoFlowControl);

    connect(batterySerial, &QSerialPort::readyRead, this, &MainWindow::readBatteryData);

    if (batterySerial->open(QIODevice::ReadOnly)) {
        qDebug() << "Battery serial port opened successfully.";
    } else {
        qDebug() << "Failed to open battery serial port.";
    }

    batteryTimer->start(2000);  // Update historical data every 2 seconds
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

        // Update QLCDNumber untuk kecepatan
        ui->speedLcd->display(sensorData.at(6).toInt());
    }
}

void MainWindow::readBatteryData() {
    QByteArray data = batterySerial->readAll();
    qDebug() << "Data received from battery serial port:" << data;

    QString dataString(data);
    QStringList values = dataString.split(',');

    qDebug() << "Parsed values:" << values;

    if (values.size() == 6) {
        float voltage = values[0].toFloat(); // Misalkan nilai pertama adalah tegangan
        int batteryPercentage = static_cast<int>((voltage / 4.2) * 100); // Konversi tegangan ke persentase

        qDebug() << "Battery voltage:" << voltage;
        qDebug() << "Battery percentage:" << batteryPercentage;

        ui->batteryLabel->setValue(batteryPercentage);
    }
}
