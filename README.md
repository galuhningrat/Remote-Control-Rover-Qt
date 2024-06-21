# Sistem Kendali Jarak Jauh untuk Kendaraan Otonom

## Deskripsi Proyek

Proyek ini merupakan implementasi sistem kendali jarak jauh untuk kendaraan otonom, spesifik untuk aplikasi rover, dengan memanfaatkan framework Qt Creator. Sistem ini mengintegrasikan berbagai modul sensor dan aktuator, termasuk motor T.T, modul kamera, modul baterai, modul GPS, akselerometer, IMU (Inertial Measurement Unit), dan sensor ultrasonik. Antarmuka grafis (GUI) yang dikembangkan memungkinkan pengguna untuk mengontrol dan memonitor berbagai aspek operasional kendaraan secara real-time.

## Fitur Utama

1. Kontrol pergerakan kendaraan (maju, mundur, belok kiri, belok kanan)
2. Pengaturan kecepatan dinamis
3. Visualisasi data sensor secara real-time
4. Komunikasi serial untuk pertukaran data dengan mikrokontroler

## Struktur Proyek

Proyek ini terdiri dari beberapa komponen kunci:

- `main.cpp`: Titik masuk aplikasi
- `mainwindow.h`: Deklarasi kelas MainWindow
- `mainwindow.cpp`: Implementasi fungsionalitas MainWindow
- `mainwindow.ui`: Definisi antarmuka pengguna

## Analisis Kode

### Konfigurasi Qt

```qmake
QT += core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = UserRemoteControl
TEMPLATE = app
```

Konfigurasi ini mengindikasikan penggunaan modul core, gui, dan serialport dari Qt. Untuk versi Qt 5 ke atas, modul widgets juga diinklusikan.

### Deklarasi Kelas MainWindow

```cpp
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
    void changeSpeed(int speed);
    void updateSensorData();
private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
};
```

Kelas MainWindow mewarisi QMainWindow dan mendefinisikan metode-metode esensial untuk kontrol kendaraan dan pembaruan data sensor.

### Implementasi MainWindow

Konstruktor MainWindow menginisialisasi antarmuka pengguna dan mengkonfigurasi koneksi serial:

```cpp
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Koneksi sinyal dan slot
    // ...
    
    // Konfigurasi port serial
    serial = new QSerialPort(this);
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->open(QIODevice::ReadWrite);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::updateSensorData);
}
```

### Metode Kontrol Kendaraan

Metode-metode seperti `moveForward()`, `moveBackward()`, `turnLeft()`, dan `turnRight()` mengimplementasikan logika kontrol dasar dengan mengirimkan perintah melalui koneksi serial.

### Pembaruan Data Sensor

Metode `updateSensorData()` memproses data yang diterima dari koneksi serial dan memperbarui antarmuka pengguna:

```cpp
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
```

## Konklusi

Sistem kendali jarak jauh yang diimplementasikan memanfaatkan kapabilitas Qt untuk menciptakan antarmuka pengguna yang responsif dan intuitif. Integrasi komunikasi serial memungkinkan interaksi real-time dengan perangkat keras kendaraan otonom. Struktur modular kode memfasilitasi maintainabilitas dan ekstensi fungsionalitas di masa depan.
