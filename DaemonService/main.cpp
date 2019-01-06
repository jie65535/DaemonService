#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include "daemonservice.h"
#include "log.h"

void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 使用共享内存，防止程序重复启动
    QSharedMemory singleton(a.applicationName());
    if(!singleton.create(1))
    {
        QMessageBox::warning(nullptr, "Waring", "Program already running!（服务已经启动，请不要重复启动服务）");
        return 1;
    }

    //注册MsgHandler回调函数
    qInstallMessageHandler(customMessageHandler);

    MainWindow w;
    w.show();
    return a.exec();
}

// 日志
void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txtMessage;

    switch (type)
    {
    case QtDebugMsg:    //调试信息提示
        txtMessage = QString("%1 Debug（调试）:\t%2").arg(QDateTime::currentDateTime().toString("yy/MM/dd HH:mm:ss.zzz")).arg(str);
        break;

    case QtWarningMsg:    //一般的warning提示
        txtMessage = QString("%1 Warning（警告）:\t%2").arg(QDateTime::currentDateTime().toString("yy/MM/dd HH:mm:ss.zzz")).arg(str);
        break;

    case QtCriticalMsg:    //严重错误提示
        txtMessage = QString("%1 Critical（错误）:\t%2").arg(QDateTime::currentDateTime().toString("yy/MM/dd HH:mm:ss.zzz")).arg(str);
        break;

    case QtFatalMsg:    //致命错误提示
        txtMessage = QString("%1 Fatal（致命错误）:\t%2").arg(QDateTime::currentDateTime().toString("yy/MM/dd HH:mm:ss.zzz")).arg(str);
        break;

    default:
        return;
    }

    Log::instance().append(txtMessage);

    if (type == QtFatalMsg)
        abort();
}
