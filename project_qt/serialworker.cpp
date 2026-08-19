#include "serialworker.h"
#include <QDebug>

SerialWorker::SerialWorker(QObject *parent)
    : QObject(parent)
{
}

SerialWorker::~SerialWorker()
{
    if (serial) {
        if (serial->isOpen()) {
            serial->close();
        }
        delete serial;
        serial = nullptr;
    }
}

void SerialWorker::openPort()
{
    if (!serial) {
        serial = new QSerialPort(this);
        connect(serial, &QSerialPort::readyRead, this, &SerialWorker::readData);
    }

    if (serial->isOpen()) {
        emit connectionStatusChanged(true);
        return;
    }

    const auto portList = QSerialPortInfo::availablePorts();
    if (portList.isEmpty()) {
        qDebug() << "연결 가능한 시리얼 포트가 없습니다.";
        emit connectionStatusChanged(false);
        return;
    }

    QString targetPort = portList.first().portName();
    serial->setPortName(targetPort);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "시리얼 포트 연결 성공:" << targetPort;
        emit connectionStatusChanged(true);
    } else {
        qDebug() << "시리얼 포트 연결 실패:" << serial->errorString();
        emit connectionStatusChanged(false);
    }
}

void SerialWorker::closePort()
{
    if (serial && serial->isOpen()) {
        serial->write("!STOP#\n");
        serial->waitForBytesWritten(100);
        serial->close();
        qDebug() << "시리얼 포트 연결 종료";
    }
    emit connectionStatusChanged(false);
}

void SerialWorker::writeData(const QByteArray &data)
{
    if (serial && serial->isOpen()) {
        serial->write(data);
    }
}

void SerialWorker::readData()
{
    while (serial && serial->canReadLine()) {
        QByteArray line = serial->readLine().trimmed();
        QString strData = QString::fromUtf8(line);
        emit dataReceived(strData);
    }
}