#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "fileOperation/jsonreader.h"

#include "task/taskoperatons.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serial(new SerialPort(this))
{
    qRegisterMetaType<QTextCursor>("QTextCursor");
    configureSettings();
    getPortsAvailable();
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
}

void MainWindow::configureSettings()
{
    ui->setupUi(this);
    connect(ui->pushButtonChooseFile, &QPushButton::clicked, this, &MainWindow::readDataFile);
    connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::startThread);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->pushButtonConfig, &QPushButton::clicked, [this]()
    {
        if(ui->groupBoxSettings->isVisible()) {
            ui->groupBoxSettings->hide();
        } else {
            ui->groupBoxSettings->show();
        }
    });

    connect(ui->pushButtonConnect, &QPushButton::clicked, this, &MainWindow::connectToDevice);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &MainWindow::disconnectFromDevice);
    connect(ui->pushButtonRefresh, &QPushButton::clicked, this, &MainWindow::getPortsAvailable);
    connect(m_serial.data(), &SerialPort::sendInformation, this, &MainWindow::showStatus);

    connect(m_serial.data(), &SerialPort::dataSended, this, &MainWindow::dataSended);
    connect(m_serial.data(), &SerialPort::sendDataReceived, this, &MainWindow::sendDataReceived);

    connect(this, &MainWindow::sendSequenceToCom, m_serial.data(), &SerialPort::deviceWrite);


    ui->groupBoxSettings->hide();
    ui->pushButtonStart->hide();
    ui->pushButtonStop->hide();
    ui->pushButtonDisconnect->hide();
}

void MainWindow::showStatus(bool status, const QString &information)
{
    m_isConnectionActive = status;
    ui->pushButtonConnect->setVisible(!m_isConnectionActive);
    ui->pushButtonDisconnect->setVisible(m_isConnectionActive);
    ui->statusbar->showMessage(information, 5000);
}

void MainWindow::startThread()
{
    QThread *threadTask;
    TaskOperatons *task;

    threadTask = new QThread;
    task = new TaskOperatons(nullptr, m_sequences);

    task->moveToThread(threadTask);

    connect(threadTask, &QThread::started, task, &TaskOperatons::startTask);
    connect(task, &TaskOperatons::sendResponseAwaited, this, &MainWindow::getAwaitedResponse, Qt::DirectConnection);
    connect(task, &TaskOperatons::sendSequenceName, ui->textBrowserInformation, &QTextBrowser::append);
    connect(task, &TaskOperatons::sendTestCaseName, ui->textBrowserInformation, &QTextBrowser::append);
    connect(this, &MainWindow::resumeTask, task, &TaskOperatons::resumeTask, Qt::DirectConnection);

    connect(task, &TaskOperatons::sendSequence, this, &MainWindow::getSequenceToSend);
    connect(this, &MainWindow::sendDataReceived, this, &MainWindow::getResponseFromDevice);

    connect(task, &TaskOperatons::finished, threadTask, &QThread::quit);
    connect(threadTask, &QThread::finished, threadTask, &QThread::deleteLater);
    connect(task, &TaskOperatons::finished, task, &TaskOperatons::deleteLater);

    threadTask->start();
}

void MainWindow::connectToDevice()
{
    if(ui->radioButtonCOM->isChecked()) {
        m_serial->deviceConnect(ui->comboBoxPortName->currentText(),
                                ui->comboBoxPortSpeed->currentText().toInt(),
                                ui->comboBoxParity->currentIndex(),
                                ui->comboBoxFlowControl->currentIndex(),
                                ui->comboBoxDataBits->currentText().toInt(),
                                ui->comboBoxStopBits->currentText().toInt()
                                );
    } else {

    }
}

void MainWindow::disconnectFromDevice()
{
    if(ui->radioButtonCOM->isChecked()) {
        m_serial->deviceDisconnect();
    } else {

    }
}

void MainWindow::getPortsAvailable()
{
    if(m_isFirstPortsRead) {
        connect(ui->comboBoxPortName, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index) {
                QString tooltip = ui->comboBoxPortName->itemData(index, Qt::ToolTipRole).toString();
                ui->comboBoxPortName->setToolTip(tooltip);
            });
        m_isFirstPortsRead = false;
    }
    ui->comboBoxPortName->clear();
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for(auto& el: serialPortInfos) {
        ui->comboBoxPortName->addItem(el.portName());
        ui->comboBoxPortName->setItemData(ui->comboBoxPortName->count()-1, el.description() + " | " + el.manufacturer(), Qt::ToolTipRole);
    }
    ui->comboBoxPortName->setToolTip(ui->comboBoxPortName->itemData(0, Qt::ToolTipRole).toString());
}

void MainWindow::getSequenceToSend(const QString &sequence)
{
    if(m_isConnectionActive) {
        if(ui->radioButtonCOM->isChecked()) {
            emit sendSequenceToCom(convertSequenceToHex(sequence));
        } else {
            emit sendSequenceToSocket(convertSequenceToHex(sequence));
        }
    }
}

QByteArray MainWindow::convertSequenceToHex(const QString &sequence)
{
    return QByteArray::fromHex(sequence.toLatin1());
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
        ui->textBrowserInformation->append(tab + tab + "<span style='color: green; font: bold;'>Poprawnie</span>");
        ui->textBrowserInformation->append("");
    } else {
        ui->textBrowserInformation->append(tab + tab + "<span style='color: red; font: bold;'>\tNiepoprawnie</span>");
        ui->textBrowserInformation->append(tab + tab + "<span style='color: red; font: italic;'>\tOczekiwana: " + responseAwaited + "</span>");
        ui->textBrowserInformation->append(tab + tab + "<span style='color: red; font: italic;'>\tOtrzymana:  " + responseReceived.toHex().toUpper() + "</span>");
        ui->textBrowserInformation->append("");
    }
    emit resumeTask();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    close();
}

