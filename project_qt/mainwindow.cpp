#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    steeringTimer = new QTimer(this);

    connect(ui->dialSteering,&QDial::valueChanged,this,&MainWindow::steeringChanged);

    serial = new QSerialPort(this);
    serial_connect();
    connect(serial,&QSerialPort::readyRead,this,&MainWindow::readSerialData);

    // 1. 백그라운드에서 카메라를 구동할 스레드 객체 생성
    camera_thread = new CameraThread(this);

    // 2. 스레드(영상 송신)와 메인 UI(영상 수신)를 시그널-슬롯으로 연결
    connect(camera_thread, SIGNAL(send_image(const QImage&)),
            this, SLOT(handle_data(const QImage&)));

    ui->dialSteering->setRange(-90, 90);
    ui->dialSteering->setValue(0);
    ui->dialSteering->setWrapping(false);

    connect(ui->dialSteering,
            &QDial::valueChanged,
            this,
            [this](int value)
    {
        ui->steeringWheel->setAngle(value * 1.7);
    });

    ui->steeringWheel->setAngle(0);

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
    delete ui;
    delete serial;
    delete camera_thread;
}

void MainWindow::on_btnDrive_clicked()
{
    // 현재 R인데 바로 D로 바꾸려고 하는 경우
    if (currentGear == 'R')
    {
        // 속도가 0이 아니면 변경 금지
        if (currentSpeed != 0)
        {
            ui->btnDrive->setChecked(false);
            ui->btnReverse->setChecked(true);

            QMessageBox::warning(
                        this,
                        "기어 변경 불가",
                        "차량이 정지한 후 D 기어로 변경해주세요."
                        );

            return;
        }
    }

    // 기어 변경
    currentGear = 'F';
    qDebug() << currentGear;
    ui->btnNeutral->setChecked(false);
    ui->btnReverse->setChecked(false);

    if (serial->isOpen()) {
        QString cmd = QString("D%1\n").arg(currentGear); // 예: "DF\n"
        serial->write(cmd.toUtf8());
    }
}

void MainWindow::on_btnNeutral_clicked()
{
    currentGear = 'N';

    ui->btnReverse->setChecked(false);
    ui->btnDrive->setChecked(false);

    if(!serial->isOpen())
        return;

    if (serial->isOpen()) {
        QString cmd = QString("D%1\n").arg(currentGear); // 예: "DF\n"
        serial->write(cmd.toUtf8());
    }
}

void MainWindow::on_btnReverse_clicked()
{
    // 현재 D인데 바로 R로 바꾸려고 하는 경우
    if (currentGear == 'F')
    {
        // 속도가 0이 아니면 변경 금지
        if (currentSpeed != 0)
        {
            ui->btnReverse->setChecked(false);
            ui->btnDrive->setChecked(true);
            QMessageBox::warning(
                        this,
                        "기어 변경 불가",
                        "차량이 정지한 후 R 기어로 변경해주세요."
                        );

            return;
        }
    }

    // 기어 변경
    currentGear = 'R';
    ui->btnDrive->setChecked(false);
    ui->btnNeutral->setChecked(false);

    if (serial->isOpen()) {
        QString cmd = QString("D%1\n").arg(currentGear); // 예: "DF\n"
        serial->write(cmd.toUtf8());
    }
}

void MainWindow::on_btnhazards_toggled(bool checked)
{
    if(checked)
    {
        ui->btnLeft->setChecked(false);
        ui->btnRight->setChecked(false);
    }

    if (serial->isOpen()) {
        QString data = QString("F%1\n").arg("E"); // 예: "FE\n"
        serial->write(data.toUtf8());
    }
}

void MainWindow::on_btnLeft_toggled(bool checked)
{
    if(checked)
    {
        ui->btnhazards->setChecked(false);
        ui->btnRight->setChecked(false);
        leftCount++;
    }

    if (serial->isOpen()) {
        QString data = QString("F%1\n").arg("L"); // 예: "FL\n"
        serial->write(data.toUtf8());
    }
}

void MainWindow::on_btnRight_toggled(bool checked)
{
    if(checked)
    {
        ui->btnLeft->setChecked(false);
        ui->btnhazards->setChecked(false);
        rightCount++;
    }

    if (serial->isOpen()) {
        QString data = QString("F%1\n").arg("R"); // 예: "FR\n"
        serial->write(data.toUtf8());
    }
}

void MainWindow::on_slidespeed_sliderReleased()
{
    if(serial->isOpen())
    {
        QString data = QString("S%1\n").arg(speedvalue); // 예: "S90\n"
        maxSpeed = qMax(maxSpeed, speedvalue);

        speedSum += speedvalue;
        speedCount++;
        serial->write(data.toUtf8());

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

    if (serial->isOpen()) {
        QString data = QString("A%1\n").arg(steeringvalue); // 예: "A50\n"
        serial->write(data.toUtf8());

        qDebug() << "STEER TX :" << data;
    }
}

void MainWindow::on_btnStart_clicked()
{
    maxSpeed = 0;
    speedSum = 0;
    speedCount = 0;

    leftCount = 0;
    rightCount = 0;

    if(serial->isOpen())
    {
        qDebug() << "Already connected";
        return;
    }

    if(serial->open(QIODevice::ReadWrite))
    {
        startTime = QDateTime::currentDateTime();
        qDebug() << "M4 CONNECT SUCCESS";
        // 3. 카메라 스레드 실행 (CameraThread::run() 함수가 호출됨)
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
    }
    else
    {
        qDebug() << "M4 CONNECT FAIL";
    }
}

void MainWindow::on_btnStop_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "종료 확인", "정말 종료 하시겠습니까?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if(serial->isOpen())
        {
            // 필요하면 연결 끊기 전에 M4에 정지 명령 전송
            serial->write("!STOP#\n");

            serial->waitForBytesWritten(100);

            serial->close();
        }
        endTime = QDateTime::currentDateTime();
        qDebug() << "M4 DISCONNECTED";
        steeringTimer->stop();

        ui->lblSpeed->setText("0");
        ui->dialSpeed->setValue(0);

        if (camera_thread) {
            camera_thread->quit(); // 1. 스레드의 이벤트 루프(exec())를 빠져나오도록 종료 신호를 보냄
            camera_thread->wait(); // 2. 스레드가 완전히 종료(run() 함수가 완전히 끝날 때)될 때까지 메인 스레드가 대기
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

        qint64 seconds =
                startTime.secsTo(endTime);

        int hour = seconds / 3600;
        int min = (seconds % 3600) / 60;
        int sec = seconds % 60;

        QString driveTime =
                QString("%1:%2:%3")
                .arg(hour, 2, 10, QChar('0'))
                .arg(min, 2, 10, QChar('0'))
                .arg(sec, 2, 10, QChar('0'));

        double avgSpeed = 0;

        if(speedCount > 0)
        {
            avgSpeed =
                    (double)speedSum / speedCount;
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
    else return;
}

void MainWindow::readSerialData()
{
    while (serial->canReadLine()) {
        QByteArray line = serial->readLine().trimmed();
        QString strData = QString::fromUtf8(line);

        qDebug() << "Received:" << strData;

        // '$'로 시작하는 정상 패킷인지 확인
        if (strData.startsWith("$")) {
            QString cleanData = strData.mid(1); // 앞의 '$' 제거
            QStringList tokens = cleanData.split(",");

            if (tokens.size() >= 2) {
                // 1. 에러 패킷이 들어온 경우 ($ERR,에러종류)
                if (tokens[0] == "ERR") {
                    ui->lblTemp->setText("Temp: Error");
                    ui->lblHumidity->setText("Humi: Error");
                }
                // 2. 정상 온습도 데이터가 들어온 경우 ($온도,습도)
                else {
                    ui->lblTemp->setText(QString("%1 °C").arg(tokens[0]));
                    ui->lblHumidity->setText(QString("%1 %").arg(tokens[1]));
                }
            }
        }
    }
}

void MainWindow::handle_data(const QImage &image) {
    // 1. 스레드로부터 받은 QImage를 화면 출력용 클래스인 QPixmap으로 변환
    QPixmap pixmap = QPixmap::fromImage(image);

    // 2. 영상을 출력할 라벨(lblImg)이 안전하게 존재한다면 이미지를 라벨에 셋팅
    if (serial->isOpen()) {
        ui->lblCamera->setText("");
        ui->lblREC->setText("● REC");
        ui->lblREC->setStyleSheet("color: #FF3B30");
        ui->lblREC->setStyleSheet("background-color: transparent;");
        ui->lblCamera->setPixmap(pixmap);
    }
}

// ============================================================================
// 윈도우 창 닫기 이벤트 핸들러 (X 버튼을 누르거나 프로그램을 종료할 때 실행)
// ============================================================================
void MainWindow::closeEvent(QCloseEvent *event) {
    // 카메라 스레드가 동작 중이라면 안전하게 종료 절차를 밟음
    if (camera_thread) {
        camera_thread->quit(); // 1. 스레드의 이벤트 루프(exec())를 빠져나오도록 종료 신호를 보냄
        camera_thread->wait(); // 2. 스레드가 완전히 종료(run() 함수가 완전히 끝날 때)될 때까지 메인 스레드가 대기
    }

    if (serial->isOpen())
    {
        serial->close();
    }

    serial->close();
    // 창 닫기 이벤트를 수락하여 프로그램을 최종적으로 종료시킴
    event->accept();
}

void MainWindow::serial_connect(void)
{
    const auto portList = QSerialPortInfo::availablePorts();
    if (portList.isEmpty()) {
        qDebug() << "연결 가능한 시리얼 포트가 없습니다.";
        return;
    }

    QString targetPort = portList.first().portName(); // 감지된 첫 번째 유효 포트 선택
    serial->setPortName(targetPort);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

