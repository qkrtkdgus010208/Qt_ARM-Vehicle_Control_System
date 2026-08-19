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
#include <serialworker.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    // 워커 스레드로 작업을 지시할 시그널
    void requestOpenSerial();
    void requestCloseSerial();
    void requestSendSerial(const QByteArray &data);

private slots:

    void on_btnDrive_clicked();
    void on_btnNeutral_clicked();
    void on_btnReverse_clicked();

    void on_btnLeft_toggled(bool checked);
    void on_btnRight_toggled(bool checked);
    void on_btnhazards_toggled(bool checked);

    void on_btnStart_clicked();
    void on_btnStop_clicked();

    void on_slidespeed_sliderReleased();
    void on_slidespeed_valueChanged(int value);
    void on_dialSpeed_valueChanged(int value);
    void steeringChanged(int value);

    // 워커로부터 결과를 전달받을 슬롯
    void handleSerialData(const QString &strData);
    void handleSerialStatus(bool isConnected);
    void handle_data(const QImage &image);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    QThread *serialThread;
    SerialWorker *serialWorker;
    bool isSerialConnected = false;

    CameraThread * camera_thread;
    QTimer *steeringTimer;

    char currentGear = 'N';
    int currentSpeed = 0;
    int speedvalue = 0;
    int steeringvalue = 0;
    int maxSpeed = 0;
    int speedSum = 0;
    int speedCount = 0;
    int leftCount = 0;
    int rightCount = 0;
    int currentTemp = 0;
    int currentHumidity = 0;
    QDateTime startTime;
    QDateTime endTime;
};
#endif // MAINWINDOW_H
