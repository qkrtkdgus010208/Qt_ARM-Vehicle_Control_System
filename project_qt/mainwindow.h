#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include "drivelog.h"
#include <QDateTime>
#include <QLabel>
#include <QCloseEvent>
#include "camerathread.h"
#include "steeringwheel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    void on_btnDrive_clicked();

    void on_btnNeutral_clicked();

    void on_btnReverse_clicked();

    void on_btnLeft_toggled(bool checked);

    void on_btnRight_toggled(bool checked);

    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void readSerialData();

    void handle_data(const QImage &image);

    void closeEvent(QCloseEvent *event);

    void on_slidespeed_sliderReleased();

    void on_slidespeed_valueChanged(int value);

    void on_dialSpeed_valueChanged(int value);

    void on_btnhazards_toggled(bool checked);

    void steeringChanged(int value);

    void sendSteering();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;

    QTimer *steeringTimer;
    int speedvalue = 0;
    int steeringvalue = 0;

    QByteArray rxBuffer;

    CameraThread * camera_thread;

    char currentGear = 'N';
    int currentSpeed = 0;

    int maxSpeed = 0;
    int speedSum = 0;
    int speedCount =0;

    int leftCount =0;
    int rightCount = 0;

    QDateTime startTime;
    QDateTime endTime;

    int currentTemp = 0;
    int currentHumidity = 0;

    void serial_connect(void);
};
#endif // MAINWINDOW_H
