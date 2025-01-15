#pragma once

#include <QCloseEvent>
#include <QDialog>
#include <QString>
#include <QByteArray>
#include <QSerialPortInfo>

#include "serialport.h"

namespace Ui {
class CommunicationSettings;
}

class CommunicationSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CommunicationSettings(QWidget *parent = nullptr);
    ~CommunicationSettings();

private:
    Ui::CommunicationSettings *ui;
    void configurateSignals();
    QScopedPointer<SerialPort> m_serial;
    bool m_isConnectionActive;
    bool m_isFirstPortsRead{true};

public slots:
    void getSequenceToSend(const QString &sequence);

private slots:
    QByteArray convertSequenceToHex(const QString &sequence);
    void connectToDevice();
    void disconnectFromDevice();
    void getPortsAvailable();
    void showStatus(bool status, const QString &information);
    void closeEvent(QCloseEvent* event);

signals:
    void connectCom(const QString&, int, int, int, int, int);
    void discconectCom();
    void connectSocket(const QString&, int);
    void discconectSocket();

    void sendSequenceToCom(const QByteArray&);
    void sendSequenceToSocket(const QByteArray&);

    void dataSended();
    void sendDataReceived(const QByteArray&);
};
