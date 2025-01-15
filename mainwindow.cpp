#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "fileOperation/jsonreader.h"

#include "task/taskoperatons.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cfgWindow(new CommunicationSettings(this))
{
    qRegisterMetaType<QTextCursor>("QTextCursor");
    configureSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readDataFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Wybierz plik");
    ui->labelFilePath->setText(filePath.split("/").last());
    QScopedPointer<JsonReader> json(new JsonReader(filePath));
    m_sequences = json->loadSequencesFromJson();
    ui->pushButtonStart->show();
    ui->label->show();
}

void MainWindow::configureSettings()
{
    ui->setupUi(this);
    connect(ui->pushButtonChooseFile, &QPushButton::clicked, this, &MainWindow::readDataFile);
    connect(ui->pushButtonConfig, &QPushButton::clicked, m_cfgWindow.data(), &CommunicationSettings::show);

    connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::startThread);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &MainWindow::close);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::resumeTask);

    ui->pushButtonStart->hide();
    ui->pushButtonStop->hide();
    ui->label->hide();
}

void MainWindow::startThread()
{
    QThread *threadTask;
    TaskOperatons *task;

    threadTask = new QThread;
    task = new TaskOperatons(nullptr, m_sequences);

    task->moveToThread(threadTask);

    connect(threadTask, &QThread::started, task, &TaskOperatons::startTask);

    connect(task, &TaskOperatons::sendSequence, m_cfgWindow.data(), &CommunicationSettings::getSequenceToSend);
    connect(m_cfgWindow.data(), &CommunicationSettings::sendDataReceived, this, &MainWindow::getResponseFromDevice);
    connect(task, &TaskOperatons::sendResponseAwaited, this, &MainWindow::getAwaitedResponse, Qt::DirectConnection);

    connect(this, &MainWindow::resumeTask, task, &TaskOperatons::resumeTask, Qt::DirectConnection);

    connect(task, &TaskOperatons::finished, threadTask, &QThread::quit);
    connect(threadTask, &QThread::finished, threadTask, &QThread::deleteLater);
    connect(task, &TaskOperatons::finished, task, &TaskOperatons::deleteLater);

    threadTask->start();
}

void MainWindow::getResponseFromDevice(const QByteArray &responseReceived)
{
    checkResponses(responseReceived);
}

void MainWindow::getAwaitedResponse(const QString &responseReceived)
{
    m_responseAwaited = responseReceived;
}

void MainWindow::checkResponses(const QByteArray &responseReceived)
{
    QByteArray responseAwaited = m_responseAwaited.replace(" ", "").toUpper().toLatin1();

    if(responseAwaited == responseReceived.toHex().toUpper()) {
        ui->textBrowserInformation->append("<span style='color: green; font: bold;'>Poprawnie</span>");
    } else {
        ui->textBrowserInformation->append("<span style='color: red; font: bold;'>Niepoprawnie</span>");
        ui->textBrowserInformation->append("<span style='color: red; font: italic;'>Oczekiwana: " + responseAwaited + "</span>");
        ui->textBrowserInformation->append("<span style='color: red; font: italic;'>Otrzymana:  " + responseReceived.toHex().toUpper() + "</span>");
    }
    emit resumeTask();
}

