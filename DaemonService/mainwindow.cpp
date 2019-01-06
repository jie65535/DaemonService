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
    setWindowIcon(QIcon(":/Daemon.ico"));
    connect(&Log::instance(), &Log::appendEvent, this, &MainWindow::log_append);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->pushButton->text() == "启动服务")
    {
        QString temp = ui->txtPortList->toPlainText().trimmed();
        if (temp.isEmpty())
        {
            log_append("Please enter the port number to be guarded！（请输入要保护的端口号！）");
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
                log_append("Illegal input detected! Please enter the correct port number!（检测到非法输入！ 请输入正确的端口号！）");
                return;
            }

            portList.append(port);
        }
        if (portList.isEmpty())
        {
            log_append("Please enter the port number to be guarded！（请输入要保护的端口号！）");
            return;
        }
        DaemonService::instance().setPortList(portList);
        qDebug("The service is starting up...（服务正在启动中...）");
        // 开始监听，绑定端口为8796
        if (DaemonService::instance().listen(QHostAddress::AnyIPv4, 8796))
        {
            qDebug("Service started successfully!（服务启动成功！）");
            ui->txtPortList->setReadOnly(true);
            ui->pushButton->setText("关闭服务");
        }
        else
        {
            qCritical() << ("Service startup failed with error message（服务启动失败，错误消息：）:" + DaemonService::instance().errorString());
        }
    }
    else
    {
        qDebug("The service is shutting down...（服务正在关闭...）");
        DaemonService::instance().close();
        ui->txtPortList->setReadOnly(false);
        ui->pushButton->setText("启动服务");
        qDebug("Service closed!（服务已关闭）");
    }
}

void MainWindow::log_append(QString msg)
{
    ui->txtLog->append(msg);
}
