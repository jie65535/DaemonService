#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "daemonservice.h"
#include "log.h"
#include <QString>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&Log::instance(), &Log::appendEvent, this, &MainWindow::log_append);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString temp = ui->txtPortList->toPlainText().trimmed();
    if (temp.isEmpty())
    {
        log_append("Please enter the port number to be guarded！");
        return;
    }
    QTextStream ts(&temp);
    QList<int> portList;
    int port = 0;
    while (!ts.atEnd())
    {
        ts >> port;

        if (port < 1 || port > 65535)
        {
            log_append("Illegal input detected! Please enter the correct port number!");
            return;
        }

        portList.append(port);
    }
    if (portList.isEmpty())
    {
        log_append("Please enter the port number to be guarded！");
        return;
    }
    DaemonService::instance().setPortList(portList);
    qDebug("The service is starting up...");
    // 开始监听，绑定端口为8796
    if (DaemonService::instance().listen(QHostAddress::AnyIPv4, 8796))
    {
        qDebug("Service started successfully!");
        ui->txtPortList->setReadOnly(true);
        ui->pushButton->setEnabled(false);
    }
    else
    {
        qCritical() << ("Service startup failed with error message:" + DaemonService::instance().errorString());
    }
}

void MainWindow::log_append(QString msg)
{
    ui->txtLog->append(msg);
}
