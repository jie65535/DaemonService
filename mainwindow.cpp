#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "daemonservice.h"
#include "log.h"
#include <QString>
#include <QDebug>
#include "dal.h"
#include "ipsechelper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Daemon.ico"));
    connect(&Log::instance(), &Log::appendEvent, this, &MainWindow::log_append);

    ui->txtPortList->setPlainText(DAL::instance().getPortList());
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<int> MainWindow::getInputPortList()
{
    QList<int> portList;
    QString temp = ui->txtPortList->toPlainText().trimmed();
    if (temp.isEmpty())
    {
        log_append("Please enter the port number to be guarded！（请输入要保护的端口号！）");
        return portList;
    }
    QTextStream ts(&temp);
    int port = 0;
    while (!ts.atEnd())
    {
        ts >> port;

        if (port < 1 || port > 65535)
        {
            log_append("Illegal input detected! Please enter the correct port number!（检测到非法输入！ 请输入正确的端口号！）");
            return portList;
        }

        portList.append(port);
    }
    return portList;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->pushButton->text() == "启动服务")
    {
        QList<int> portList = getInputPortList();
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
            DAL::instance().setPortList(ui->txtPortList->toPlainText());
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

void MainWindow::on_btnClosePort_clicked()
{
    QList<int> portList = getInputPortList();
    if (portList.isEmpty())
    {
        log_append("Please enter the port number to be closed！（请输入要拦截的端口号！）");
        return;
    }
    for (int port : portList)
    {
        qDebug("正在检查端口：%d 是否已存在拦截列表", port);
        if (DAL::instance().isExistsBlackList(port))
        {
            qDebug("该端口已存在拦截列表，跳过操作");
            continue;
        }
        else
        {
            qDebug("该端口不存在拦截列表，开始添加到安全策略...");
            IpsecHelper::addItemToBlackList(port);
            DAL::instance().addItemToBlackList(port);
            qDebug("添加完成");
        }
    }
}

void MainWindow::on_BtnClear_clicked()
{
    ui->txtLog->clear();
}

void MainWindow::on_btnAddIP_clicked()
{
    QString ip = ui->txtIP->text();
    if (ip.isEmpty())
    {
        log_append("请输入要加入白名单的IP");
        return;
    }
    QList<int> portList = getInputPortList();
    auto list = DAL::instance().getWhiteList(ip);
    for (int port : portList)
    {
        bool flag = false;
        for (const auto &item : list)
        {
            if (item.Port == port)
            {
                flag = true;
                break;
            }
        }
        if (!flag)
        {
            //qDebug("Add to whitelists...（正在将该IP添加到白名单...）");
            IpsecHelper::addItemToWhitelist(ip, port);
        }
        else
        {
            //qDebug("Update last login time...（检测到该IP已在白名单，更新其最后上线时间...）");
        }
    }
    if (DAL::instance().updateWhiteList(ip, portList))
        qDebug("IP:%s 已经添加", ip.toStdString().data());
    else
        qWarning("添加失败");
}

void MainWindow::on_btnRemoveIP_clicked()
{
    QString ip = ui->txtIP->text();
    if (ip.isEmpty())
    {
        log_append("请输入要移除白名单的IP");
        return;
    }
    QList<int> portList = getInputPortList();
    auto list = DAL::instance().getWhiteList(ip);
    for (int i = portList.length() - 1; i >= 0; --i)
    {
        bool flag = false;
        for (const auto &item : list)
        {
            if (item.Port == portList[i])
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            IpsecHelper::removeItemFromWhiteList(ip, portList[i]);
        }
        else
        {
            portList.removeAt(i);
        }
    }
    if (portList.length() < 1)
        qDebug("IP:%s 不存在白名单中", ip.toStdString().data());
    else
    {
        DAL::instance().removeFromWhiteList(ip, portList);
        qDebug("IP:%s 已移出指定端口白名单", ip.toStdString().data());
    }
}
