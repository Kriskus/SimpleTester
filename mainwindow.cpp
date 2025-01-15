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
    connect(m_cfgWindow.data(), &CommunicationSettings::dataReceived, task, &TaskOperatons::resumeTask, Qt::DirectConnection);
    //connect(m_cfgWindow.data(), &CommunicationSettings::dataReceived, task, &TaskOperatons::getResponse, Qt::DirectConnection);

    connect(task, &TaskOperatons::sendSequence, m_cfgWindow.data(), &CommunicationSettings::getSequenceToSend, Qt::DirectConnection);
    connect(task, &TaskOperatons::sendResult, ui->textBrowserInformation, &QTextBrowser::append, Qt::DirectConnection);
    connect(task, &TaskOperatons::sendTestCaseName, ui->textBrowserInformation, &QTextBrowser::append, Qt::DirectConnection);


    connect(task, &TaskOperatons::finished, threadTask, &QThread::quit);
    connect(threadTask, &QThread::finished, threadTask, &QThread::deleteLater);
    connect(task, &TaskOperatons::finished, task, &TaskOperatons::deleteLater);

    threadTask->start();
}

void MainWindow::getResponse(const QByteArray &responseReceived)
{
    qApp->processEvents();
    checkResponse(responseReceived);
    resumeTask();
}

void MainWindow::checkResponse(const QByteArray &responseReceived)
{
    // if(responseReceived.toHex() == QByteArray::fromHex(m_responseAwaited)) {
    //     emit sendResult("\t\tPoprawnie");
    // } else {
    //     emit sendResult("\t\tWystêpuj¹ ró¿nice: " + responseReceived.toHex() + " | " + QByteArray::fromHex(m_responseAwaited));
    // }
}
