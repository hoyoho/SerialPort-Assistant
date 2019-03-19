#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QValidator>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),m_serial(new QSerialPort(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon.png"));
    setWindowTitle(tr("SerialPort Assistant"));
    setFixedSize(size());



    ui->pushButtonConnect->setText(tr("Connect"));
    ui->pushButtonRefresh->setText(tr("Refresh"));
    fillPortsParameters();

    ui->plainTextEditReceive->setReadOnly(true);
    ui->labelTipCategory->setText(tr(""));
    ui->labelTipCategory->setLineWidth(ui->groupBoxSendWay->width());
    ui->labelTipContent->setText(tr(""));

    ui->radioButtonRecChar->setText(tr("By character"));
    ui->radioButtonRecChar->setChecked(true);
    ui->radioButtonRecHex->setText(tr("By hexadecimal"));

    ui->radioButtonSendChar->setText(tr("By character"));
    ui->radioButtonSendChar->setChecked(true);
    ui->radioButtonSendHex->setText(tr("By hexadecimal"));

    ui->groupBoxConfig->setTitle(tr("Config"));
    ui->groupBoxReceiveWay->setTitle(tr("Receive way"));
    ui->groupBoxSendWay->setTitle(tr("Send way"));
    ui->groupBoxReceiveArea->setTitle(tr("Receive content"));
    ui->groupBoxSendArea->setTitle(tr("Send content"));

    ui->labelGapTime->setText(tr("Period(ms)"));
    ui->checkBoxAutoSend->setText(tr("AutoSend"));
    ui->lineEditGaptime->setValidator(new QIntValidator(1,99999,this));
    changeAutoCheckState();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this,SLOT(sendOutMeaage()));

    connect(ui->lineEditGaptime, &QLineEdit::textChanged, this, &MainWindow::changeAutoCheckState);
    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillPortsParameters()
{
    ui->labelRate->setText(tr("BaudRate:"));
    ui->comboBoxRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBoxRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBoxRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBoxRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    ui->labelDataBits->setText(tr("Data bits:"));
    ui->comboBoxDataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->comboBoxDataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->comboBoxDataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->comboBoxDataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->comboBoxDataBits->setCurrentIndex(3);

    ui->labelParity->setText(tr("Parity:"));
    ui->comboBoxParity->addItem(tr("None"), QSerialPort::NoParity);
    ui->comboBoxParity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->comboBoxParity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->comboBoxParity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->comboBoxParity->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->labelStopBits->setText(tr("Stop bits:"));
    ui->comboBoxStopBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->comboBoxStopBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->labelPort->setText(tr("Seril Port:"));
    fillPortsInfo();
}



void MainWindow::changeAutoCheckState()
{
    if(ui->lineEditGaptime->hasAcceptableInput())
    {
        ui->checkBoxAutoSend->setCheckable(true);
    }
    else
    {
        ui->checkBoxAutoSend->setCheckable(false);
    }
}


void MainWindow::on_pushButtonRefresh_clicked()
{
    fillPortsInfo();
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if(m_serial->isOpen())
    {
        closeSerialPort();
        ui->pushButtonConnect->setText(tr("Connect"));
    }
    else
    {
        m_serial->setPortName(ui->comboBoxPort->currentText());
        m_serial->setBaudRate(static_cast<QSerialPort::BaudRate>(ui->comboBoxRate->itemData(ui->comboBoxRate->currentIndex()).toInt()));
        m_serial->setDataBits(static_cast<QSerialPort::DataBits>(ui->comboBoxDataBits->itemData(ui->comboBoxDataBits->currentIndex()).toInt()));
        m_serial->setParity(static_cast<QSerialPort::Parity>(ui->comboBoxParity->itemData(ui->comboBoxParity->currentIndex()).toInt()));
        m_serial->setFlowControl(QSerialPort::NoFlowControl);
        if (m_serial->open(QIODevice::ReadWrite)) {
            showStatusMessage(tr("Connection:"),tr("Connect Successful!"));
            ui->pushButtonConnect->setText(tr("Disconnect"));
        } else {
            showStatusMessage(tr("Connection:"),m_serial->errorString());
        }
    }
}


void MainWindow::sendOutMeaage()
{

    const QString sendString(ui->plainTextEditSend->toPlainText());
    if(ui->radioButtonSendHex->isChecked())
    {
        bool ok;
        QByteArray temp;
        QStringList list(sendString.simplified().split(" "));
        for(QString &e : list)
        {
            temp.append(e.toInt(&ok, 16) & 0xFF);
        }
        if(ok)
        {
            m_serial->write(temp);
        }
        else
        {
           showStatusMessage(tr("Error"), tr("Unknown hexadecimal"));
           deactiveAuto();
           QMessageBox::critical(this, tr("Error"), tr("Unknown hexadecimal"));
        }
    }
    else
    {
        m_serial->write(QByteArray(sendString.toUtf8()));
    }
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->plainTextEditReceive->clear();
}

void MainWindow::fillPortsInfo()
{
    ui->comboBoxPort->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    if(infos.size() == 0)
    {
        ui->comboBoxPort->addItem(tr("None"));
        ui->pushButtonConnect->setEnabled(false);
    }
    else
    {
        ui->pushButtonConnect->setEnabled(true);
        for (const QSerialPortInfo &info : infos)
        {
            ui->comboBoxPort->addItem(info.portName());
        }
    }
}

void MainWindow::readData()
{
    const QByteArray data(m_serial->readAll());
    if(ui->radioButtonRecHex->isChecked())
    {
        std::string temp(data.toHex().toUpper().toStdString());
        QString toBeShow;
        for(auto it = temp.begin(); it != temp.end(); ++it)
        {
            toBeShow.append(*it++).append(*it).append(" ");
        }
        ui->plainTextEditReceive->appendPlainText(toBeShow);
    }else
    {
        ui->plainTextEditReceive->insertPlainText(data);
    }
}


void MainWindow::closeSerialPort()
{
    m_serial->close();
    showStatusMessage(tr("Connection:"),tr("Disconnected"));
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::NoError)
        return;
    if (error == QSerialPort::ResourceError) {
        closeSerialPort();
    }
    QMessageBox::critical(this, tr("Error"), m_serial->errorString());
    deactiveAuto();
}

void MainWindow::showStatusMessage(const QString &category, const QString &content)
{
    ui->labelTipCategory->setText(category);
    ui->labelTipContent->setText(content);
}



void MainWindow::activeAuto()
{
    if(ui->plainTextEditSend->toPlainText().size() == 0)  return;
    timer->start(ui->lineEditGaptime->text().toInt());
    ui->pushButtonSend->setText(tr("Stop"));
    timer->start();
}
void MainWindow::deactiveAuto()
{
    ui->pushButtonSend->setText(tr("Send"));
    timer->stop();
}

void MainWindow::on_pushButtonSend_clicked()
{
    if(ui->checkBoxAutoSend->isChecked())
    {
        if(timer->isActive())
            deactiveAuto();
        else
            activeAuto();
    }
    else
        sendOutMeaage();
}
