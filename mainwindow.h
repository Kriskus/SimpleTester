#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QScopedPointer>

#include "fileOperation/sequencestructure.h"
#include "comm/communicationsettings.h"

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
    QScopedPointer<CommunicationSettings> m_cfgWindow;

    QMap<QString, Sequence> m_sequences{};

    QString m_responseAwaited{};

private slots:
    void readDataFile();
    void configureSettings();

    void startThread();

    void getResponseFromDevice(const QByteArray& responseReceived);
    void getAwaitedResponse(const QString& responseReceived);
    void checkResponses(const QByteArray& responseReceived);

signals:
    void sendData(const QString&);
    void resumeTask();
};
