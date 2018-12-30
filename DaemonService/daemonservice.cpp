#include "daemonservice.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QThread>
#include "worker.h"

//DaemonService::DaemonService()
//{
//    if (this->listen(QHostAddress::AnyIPv4, 9876))
//        qDebug("The server started successfully!");
//    else
//        qCritical("Server failed to start！Error message：%s", this->errorString().toStdString().data()); //错误信息
//}

/**
 * @brief 当有新连接进入时
 */
void DaemonService::incomingConnection(qintptr socketDescriptor)
{
    qDebug("new connect is connect %d", socketDescriptor);
    Worker *worker = new Worker(socketDescriptor, this->m_portList);
    QThreadPool::globalInstance()->start(worker);
}

