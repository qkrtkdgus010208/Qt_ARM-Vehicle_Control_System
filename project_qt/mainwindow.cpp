#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this)) // 이 부분이 있어야 nullptr 에러가 나지 않습니다.
{
    ui->setupUi(this);

    // 1. PC에 연결된 시리얼 포트 목록 검색 후 콤보박스에 추가
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->comboPorts->addItem(info.portName());
    }

    // 2. 데이터 수신 시 readData() 슬롯 호출 연결
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();
    }

    delete ui;
}

// 연결/해제 버튼
void MainWindow::on_btnConnect_clicked()
{
    if (!serial->isOpen()) {
        QString portName = ui->comboPorts->currentText();
        if (portName.isEmpty()) return;

        serial->setPortName(portName);
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        if (serial->open(QIODevice::ReadWrite)) {
            ui->btnConnect->setText("Disconnect");
            ui->comboPorts->setEnabled(false);
        } else {
            QMessageBox::critical(this, "Error", "포트를 열 수 없습니다.");
        }
    } else {
        serial->close();
        ui->btnConnect->setText("Connect");
        ui->comboPorts->setEnabled(true);
    }
}

// STM32로 데이터 송신
void MainWindow::on_btnSend_clicked()
{
    if (serial->isOpen()) {
        QString cmd = ui->editCommand->text() + "\n"; // 패킷 끝 식별자 \n 추가
        serial->write(cmd.toUtf8());
    }
}

// STM32로부터 데이터 수신
void MainWindow::readData()
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
                    ui->lblAdc->setText("Humi: " + tokens[1]);
                }
                // 2. 정상 온습도 데이터가 들어온 경우 ($온도,습도)
                else {
                    ui->lblTemp->setText("Temp: " + tokens[0] + " ℃");
                    ui->lblAdc->setText("Humi: " + tokens[1] + " %");
                }
            }
        }
    }
}