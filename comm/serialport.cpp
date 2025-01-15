#include "serialport.h"

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
    , m_serial(new QSerialPort(this))
{
    connect(m_serial.data(), &QSerialPort::readyRead, this, &SerialPort::deviceRead);
}

SerialPort::~SerialPort()
{}

void SerialPort::deviceConnect(const QString& portName, int BaudRate, int parity, int flowControl, int dataBits, int stopBits)
{
    setPortName(portName);
    setBaudRate(BaudRate);
    setParity(parity);
    setFlowControl(flowControl);
    setDataBits(dataBits);
    setStopBits(stopBits);

    if(m_serial->open(QIODevice::ReadWrite)) {
        emit sendInformation(true, "Connected");
    } else {
        emit sendInformation(false, "Failed to connect: " + m_serial->errorString());
    }
}

void SerialPort::deviceDisconnect()
{
    m_serial->close();
    emit sendInformation(false, "Disconnected");
}

void SerialPort::deviceRead()
{
    m_response.push_back(m_serial->readAll());
    if(m_response.contains("\03")) {
        emit sendDataReceived(m_response);
        m_response.clear();
    }
}

void SerialPort::deviceWrite(const QByteArray &data)
{
    m_serial->write(data);
    emit dataSended();
}

void SerialPort::setPortName(const QString &portName)
{
    m_serial->setPortName(portName);
}

void SerialPort::setParity(int parityType)
{
    switch (parityType) {
    case 0:
        m_serial->setParity(QSerialPort::NoParity); break;
    case 1:
        m_serial->setParity(QSerialPort::EvenParity); break;
    case 2:
        m_serial->setParity(QSerialPort::OddParity); break;
    default:
        emit sendInformation(false, "Invalid parity type"); break;
    }
}

void SerialPort::setFlowControl(int flowControlType)
{
    switch (flowControlType) {
    case 0:
        m_serial->setFlowControl(QSerialPort::NoFlowControl); break;
    case 1:
        m_serial->setFlowControl(QSerialPort::HardwareControl); break;
    case 2:
        m_serial->setFlowControl(QSerialPort::SoftwareControl); break;
    default:
        emit sendInformation(false, "Invalid flow control type"); break;
    }
}

void SerialPort::setBaudRate(int baudRate)
{
    switch (baudRate) {
    case 9600:
        m_serial->setBaudRate(QSerialPort::Baud9600); break;
    case 19200:
        m_serial->setBaudRate(QSerialPort::Baud19200); break;
    case 38400:
        m_serial->setBaudRate(QSerialPort::Baud38400); break;
    case 57600:
        m_serial->setBaudRate(QSerialPort::Baud57600); break;
    case 115200:
        m_serial->setBaudRate(QSerialPort::Baud115200); break;
    default:
        emit sendInformation(false, "Invalid baud rate type"); break;
    }
}

void SerialPort::setDataBits(int dataBits)
{
    switch (dataBits) {
    case 5:
        m_serial->setDataBits(QSerialPort::Data5); break;
    case 6:
        m_serial->setDataBits(QSerialPort::Data6); break;
    case 7:
        m_serial->setDataBits(QSerialPort::Data7); break;
    case 8:
        m_serial->setDataBits(QSerialPort::Data8); break;
    default:
        emit sendInformation(false, "Invalid data bits type"); break;
    }
}

void SerialPort::setStopBits(int stopBits)
{
    switch (stopBits) {
    case 0:
        m_serial->setStopBits(QSerialPort::OneStop); break;
    case 1:
        m_serial->setStopBits(QSerialPort::OneAndHalfStop); break;
    case 2:
        m_serial->setStopBits(QSerialPort::TwoStop); break;
    default:
        emit sendInformation(false, "Invalid stop bits type"); break;
    }
}
