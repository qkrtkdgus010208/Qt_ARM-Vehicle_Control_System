#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker();

public slots:
    void openPort();
    void closePort();
    void writeData(const QByteArray &data);

private slots:
    void readData();

signals:
    void dataReceived(const QString &data);
    void connectionStatusChanged(bool isConnected);

private:
    QSerialPort *serial = nullptr;
};

#endif // SERIALWORKER_H