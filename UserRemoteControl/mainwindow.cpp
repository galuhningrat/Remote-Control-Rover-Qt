#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Menghubungkan sinyal dan slot
    connect(ui->forwardButton, &QPushButton::pressed, this, &MainWindow::moveForward);
    connect(ui->backwardButton, &QPushButton::pressed, this, &MainWindow::moveBackward);
    connect(ui->leftButton, &QPushButton::pressed, this, &MainWindow::turnLeft);
    connect(ui->rightButton, &QPushButton::pressed, this, &MainWindow::turnRight);
    connect(ui->speedSlider, &QSlider::valueChanged, this, &MainWindow::changeSpeed);

    // Setup serial port
    serial = new QSerialPort(this);
    serial->setPortName("COM3"); // Sesuaikan dengan port yang Anda gunakan
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->open(QIODevice::ReadWrite);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::updateSensorData);
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

void MainWindow::changeSpeed(int speed) {
    QString command = QString("SPEED %1\n").arg(speed);
    serial->write(command.toUtf8());
}

void MainWindow::updateSensorData() {
    QByteArray data = serial->readAll();
    QList<QByteArray> sensorData = data.split(',');

    if (sensorData.size() >= 6) {
        ui->cameraLabel->setText(sensorData.at(0));
        ui->batteryLabel->setText(sensorData.at(1));
        ui->gpsLabel->setText(sensorData.at(2));
        ui->accelerometerLabel->setText(sensorData.at(3));
        ui->imuLabel->setText(sensorData.at(4));
        ui->ultrasonicLabel->setText(sensorData.at(5));
    }
}
