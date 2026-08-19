#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    steeringTimer = new QTimer(this);

    // 시리얼 워커 및 스레드 설정
    serialThread = new QThread(this);
    serialWorker = new SerialWorker(); // 부모를 지정하지 않고 생성
    serialWorker->moveToThread(serialThread);

    // 시그널-슬롯 연결 (메인 스레드 -> 워커 스레드)
    connect(this, &MainWindow::requestOpenSerial, serialWorker, &SerialWorker::openPort);
    connect(this, &MainWindow::requestCloseSerial, serialWorker, &SerialWorker::closePort);
    connect(this, &MainWindow::requestSendSerial, serialWorker, &SerialWorker::writeData);

    // 시그널-슬롯 연결 (워커 스레드 -> 메인 스레드)
    connect(serialWorker, &SerialWorker::dataReceived, this, &MainWindow::handleSerialData);
    connect(serialWorker, &SerialWorker::connectionStatusChanged, this, &MainWindow::handleSerialStatus);

    // 스레드 종료 시 워커 객체 메모리 해제
    connect(serialThread, &QThread::finished, serialWorker, &QObject::deleteLater);

    // 워커 스레드 이벤트 루프 시작
    serialThread->start();

    // 백그라운드 카메라 스레드
    camera_thread = new CameraThread(this);
    connect(camera_thread, SIGNAL(send_image(const QImage&)), this, SLOT(handle_data(const QImage&)));

    // 조향 다이얼 설정
    ui->dialSteering->setRange(-90, 90);
    ui->dialSteering->setValue(0);
    ui->dialSteering->setWrapping(false);

    connect(ui->dialSteering, &QDial::valueChanged, this, &MainWindow::steeringChanged);
    connect(ui->dialSteering, &QDial::valueChanged, this, [this](int value) {
        ui->steeringWheel->setAngle(value * 1.7);
    });

    ui->steeringWheel->setAngle(0);

    // 초기 UI 비활성화
    ui->btnDrive->setEnabled(false);
    ui->btnNeutral->setEnabled(false);
    ui->btnReverse->setEnabled(false);
    ui->btnLeft->setEnabled(false);
    ui->btnRight->setEnabled(false);
    ui->slidespeed->setEnabled(false);
    ui->dialSpeed->setEnabled(false);
    ui->dialSteering->setEnabled(false);
    ui->btnhazards->setEnabled(false);
    ui->btnKlaxon->setEnabled(false);
}

MainWindow::~MainWindow()
{
    if (serialThread->isRunning()) {
        serialThread->quit();
        serialThread->wait();
    }
    delete ui;
    delete camera_thread;
}

void MainWindow::on_btnStart_clicked()
{
    maxSpeed = 0;
    speedSum = 0;
    speedCount = 0;
    leftCount = 0;
    rightCount = 0;

    if (isSerialConnected) {
        qDebug() << "Already connected";
        return;
    }

    // 워커 스레드에 포트 열기 요청
    emit requestOpenSerial();
}

void MainWindow::handleSerialStatus(bool isConnected)
{
    isSerialConnected = isConnected;

    if (isConnected) {
        startTime = QDateTime::currentDateTime();
        qDebug() << "M4 CONNECT SUCCESS";

        camera_thread->start();
        steeringTimer->setInterval(1000);
        steeringTimer->start();

        ui->btnDrive->setEnabled(true);
        ui->btnNeutral->setEnabled(true);
        ui->btnReverse->setEnabled(true);
        ui->btnLeft->setEnabled(true);
        ui->btnRight->setEnabled(true);
        ui->slidespeed->setEnabled(true);
        ui->dialSpeed->setEnabled(false);
        ui->dialSteering->setEnabled(true);
        ui->btnhazards->setEnabled(true);
        ui->btnKlaxon->setEnabled(true);
    } else {
        qDebug() << "M4 CONNECT FAIL or DISCONNECTED";
    }
}

void MainWindow::on_btnStop_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "종료 확인", "정말 종료 하시겠습니까?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (isSerialConnected) {
            emit requestCloseSerial();
        }

        endTime = QDateTime::currentDateTime();
        steeringTimer->stop();

        ui->lblSpeed->setText("0");
        ui->dialSpeed->setValue(0);

        if (camera_thread) {
            camera_thread->quit();
            camera_thread->wait();
        }

        ui->lblCamera->setText("NO SIGNAL");
        ui->lblREC->setText("");
        ui->btnDrive->setEnabled(false);
        ui->btnNeutral->setEnabled(false);
        ui->btnReverse->setEnabled(false);
        ui->btnLeft->setEnabled(false);
        ui->btnRight->setEnabled(false);
        ui->slidespeed->setEnabled(false);
        ui->dialSpeed->setEnabled(false);
        ui->dialSteering->setEnabled(false);

        qint64 seconds = startTime.secsTo(endTime);
        int hour = seconds / 3600;
        int min = (seconds % 3600) / 60;
        int sec = seconds % 60;

        QString driveTime = QString("%1:%2:%3")
                                .arg(hour, 2, 10, QChar('0'))
                                .arg(min, 2, 10, QChar('0'))
                                .arg(sec, 2, 10, QChar('0'));

        double avgSpeed = 0;
        if (speedCount > 0) {
            avgSpeed = (double)speedSum / speedCount;
        }

        DriveLog dlg(this);
        dlg.setDriveLog(
            startTime.toString("hh:mm:ss"),
            endTime.toString("hh:mm:ss"),
            driveTime,
            maxSpeed,
            avgSpeed,
            leftCount,
            rightCount,
            currentTemp,
            currentHumidity
        );
        dlg.exec();
    }
}

void MainWindow::on_btnDrive_clicked()
{
    if (currentGear == 'R' && currentSpeed != 0) {
        ui->btnDrive->setChecked(false);
        ui->btnReverse->setChecked(true);
        QMessageBox::warning(this, "기어 변경 불가", "차량이 정지한 후 D 기어로 변경해주세요.");
        return;
    }

    currentGear = 'F';
    ui->btnNeutral->setChecked(false);
    ui->btnReverse->setChecked(false);

    if (isSerialConnected) {
        QString cmd = QString("D%1\n").arg(currentGear);
        emit requestSendSerial(cmd.toUtf8());
    }
}

void MainWindow::on_btnNeutral_clicked()
{
    currentGear = 'N';
    ui->btnReverse->setChecked(false);
    ui->btnDrive->setChecked(false);

    if (isSerialConnected) {
        QString cmd = QString("D%1\n").arg(currentGear);
        emit requestSendSerial(cmd.toUtf8());
    }
}

void MainWindow::on_btnReverse_clicked()
{
    if (currentGear == 'F' && currentSpeed != 0) {
        ui->btnReverse->setChecked(false);
        ui->btnDrive->setChecked(true);
        QMessageBox::warning(this, "기어 변경 불가", "차량이 정지한 후 R 기어로 변경해주세요.");
        return;
    }

    currentGear = 'R';
    ui->btnDrive->setChecked(false);
    ui->btnNeutral->setChecked(false);

    if (isSerialConnected) {
        QString cmd = QString("D%1\n").arg(currentGear);
        emit requestSendSerial(cmd.toUtf8());
    }
}

void MainWindow::on_btnhazards_toggled(bool checked)
{
    if (checked) {
        ui->btnLeft->setChecked(false);
        ui->btnRight->setChecked(false);
    }

    if (isSerialConnected) {
        QString data = QString("F%1\n").arg("E");
        emit requestSendSerial(data.toUtf8());
    }
}

void MainWindow::on_btnLeft_toggled(bool checked)
{
    if (checked) {
        ui->btnhazards->setChecked(false);
        ui->btnRight->setChecked(false);
        leftCount++;
    }

    if (isSerialConnected) {
        QString data = QString("F%1\n").arg("L");
        emit requestSendSerial(data.toUtf8());
    }
}

void MainWindow::on_btnRight_toggled(bool checked)
{
    if (checked) {
        ui->btnLeft->setChecked(false);
        ui->btnhazards->setChecked(false);
        rightCount++;
    }

    if (isSerialConnected) {
        QString data = QString("F%1\n").arg("R");
        emit requestSendSerial(data.toUtf8());
    }
}

void MainWindow::on_slidespeed_sliderReleased()
{
    if (isSerialConnected) {
        QString data = QString("S%1\n").arg(speedvalue);
        maxSpeed = qMax(maxSpeed, speedvalue);
        speedSum += speedvalue;
        speedCount++;

        emit requestSendSerial(data.toUtf8());
        qDebug() << "SPEED TX :" << data;
    }
}

void MainWindow::on_slidespeed_valueChanged(int value)
{
    currentSpeed = value;
    speedvalue = value;
    ui->lblSpeed->setText(QString("%1").arg(value));
    ui->dialSpeed->setValue(value);
}

void MainWindow::on_dialSpeed_valueChanged(int value)
{
    currentSpeed = value;
    speedvalue = value;
    ui->slidespeed->setValue(value);
    ui->lblSpeed->setText(QString("%1").arg(value));
}

void MainWindow::steeringChanged(int value)
{
    steeringvalue = value;

    if (isSerialConnected) {
        QString data = QString("A%1\n").arg(steeringvalue);
        emit requestSendSerial(data.toUtf8());
        qDebug() << "STEER TX :" << data;
    }
}

void MainWindow::handleSerialData(const QString &strData)
{
    qDebug() << "Received:" << strData;

    if (strData.startsWith("$")) {
        QString cleanData = strData.mid(1);
        QStringList tokens = cleanData.split(",");

        if (tokens.size() >= 2) {
            if (tokens[0] == "ERR") {
                ui->lblTemp->setText("Temp: Error");
                ui->lblHumidity->setText("Humi: Error");
            } else {
                ui->lblTemp->setText(QString("%1 °C").arg(tokens[0]));
                ui->lblHumidity->setText(QString("%1 %").arg(tokens[1]));
                currentTemp = tokens[0].toInt();
                currentHumidity = tokens[1].toInt();
            }
        }
    }
}

void MainWindow::handle_data(const QImage &image)
{
    QPixmap pixmap = QPixmap::fromImage(image);

    if (isSerialConnected) {
        ui->lblCamera->setText("");
        ui->lblREC->setText("● REC");
        ui->lblREC->setStyleSheet("color: #FF3B30; background-color: transparent;");
        ui->lblCamera->setPixmap(pixmap);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (camera_thread) {
        camera_thread->quit();
        camera_thread->wait();
    }

    if (serialThread->isRunning()) {
        emit requestCloseSerial();
        serialThread->quit();
        serialThread->wait();
    }

    event->accept();
}