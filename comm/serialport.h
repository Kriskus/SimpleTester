#pragma once

#include <QObject>
#include <QSerialPort>
#include <QScopedPointer>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

public slots:
    void deviceConnect(const QString &portName, int BaudRate, int parity, int flowControl, int dataBits, int stopBits);
    void deviceDisconnect();
    void deviceRead();
    void deviceWrite(const QByteArray& data);

    void setPortName(const QString& portName);
    void setParity(int parityType);
    void setFlowControl(int flowControlType);
    void setBaudRate(int baudRate);
    void setDataBits(int dataBits);
    void setStopBits(int stopBits);

private:
    QScopedPointer<QSerialPort> m_serial;
    QByteArray m_response{};

signals:
    void dataSended();
    void dataReceived(const QByteArray&);
    void sendInformation(bool, const QString&);
};
