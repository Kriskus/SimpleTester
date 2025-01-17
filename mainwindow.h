#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QScopedPointer>
#include <QSerialPortInfo>

#include "fileOperation/sequencestructure.h"
#include "comm/serialport.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QScopedPointer<SerialPort> m_serial;
    QMap<QString, Sequence> m_sequences{};
    QString m_responseAwaited{};
    bool m_isConnectionActive;
    bool m_isFirstPortsRead{true};

    QString tab{"&nbsp;&nbsp;&nbsp;&nbsp;"};

private slots:
    void readDataFile();
    void configureSettings();
    void showStatus(bool status, const QString &information);

    void startThread();

    void connectToDevice();
    void disconnectFromDevice();

    void getPortsAvailable();
    void getSequenceToSend(const QString &sequence);
    QByteArray convertSequenceToHex(const QString &sequence);

    void getResponseFromDevice(const QByteArray& responseReceived);
    void getAwaitedResponse(const QString& responseReceived);
    void checkResponses(const QByteArray& responseReceived);

    void closeEvent(QCloseEvent* event);

signals:
    void sendData(const QString&);
    void resumeTask();
    void dataSended();
    void sendDataReceived(const QByteArray&);
    void sendSequenceToCom(const QByteArray&);
    void sendSequenceToSocket(const QByteArray&);
};
