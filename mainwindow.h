#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushButtonRefresh_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonClear_clicked();

    void readData();
    void handleError(QSerialPort::SerialPortError error);

    void changeAutoCheckState();

    void sendOutMeaage();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void showStatusMessage(const QString &category, const QString &content);
    void closeSerialPort();

    void activeAuto();
    void deactiveAuto();


    Ui::MainWindow *ui = nullptr;
    QSerialPort *m_serial = nullptr;
    QTimer *timer = nullptr;
};

#endif // MAINWINDOW_H
