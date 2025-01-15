#include "communicationsettings.h"
#include "ui_communicationsettings.h"

CommunicationSettings::CommunicationSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CommunicationSettings)
    , m_serial(new SerialPort(this))
    , m_isConnectionActive(false)
{
    configurateSignals();
}

CommunicationSettings::~CommunicationSettings()
{
    delete ui;
}

void CommunicationSettings::configurateSignals()
{
    ui->setupUi(this);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &CommunicationSettings::close);
    connect(ui->pushButtonConnect, &QPushButton::clicked, this, &CommunicationSettings::connectToDevice);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &CommunicationSettings::disconnectFromDevice);
    connect(ui->pushButtonRefresh, &QPushButton::clicked, this, &CommunicationSettings::getPortsAvailable);
    connect(m_serial.data(), &SerialPort::sendInformation, this, &CommunicationSettings::showStatus);

    connect(m_serial.data(), &SerialPort::dataSended, this, &CommunicationSettings::dataSended);
    connect(m_serial.data(), &SerialPort::dataReceived, this, &CommunicationSettings::dataReceived);

    connect(this, &CommunicationSettings::sendSequenceToCom, m_serial.data(), &SerialPort::deviceWrite);

    getPortsAvailable();
}

void CommunicationSettings::getSequenceToSend(const QString& sequence)
{
    if(m_isConnectionActive) {
        if(ui->radioButtonCOM->isChecked()) {
            emit sendSequenceToCom(convertSequenceToHex(sequence));
        } else {
            emit sendSequenceToSocket(convertSequenceToHex(sequence));
        }
    }
}

QByteArray CommunicationSettings::convertSequenceToHex(const QString &sequence)
{
    return QByteArray::fromHex(sequence.toLatin1());
}

void CommunicationSettings::connectToDevice()
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

void CommunicationSettings::disconnectFromDevice()
{
    if(ui->radioButtonCOM->isChecked()) {
        m_serial->deviceDisconnect();
    } else {

    }
}

void CommunicationSettings::getPortsAvailable()
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

void CommunicationSettings::showStatus(bool status, const QString& information)
{
    m_isConnectionActive = status;
    ui->pushButtonConnect->setVisible(!m_isConnectionActive);
    ui->pushButtonDisconnect->setVisible(m_isConnectionActive);
    ui->labelStatus->setText(information);
}

void CommunicationSettings::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    close();
}
