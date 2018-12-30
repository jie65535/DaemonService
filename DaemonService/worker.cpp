#include <QTcpSocket>
#include <QHostAddress>
#include <QProcess>
#include <string>
#include "worker.h"
#include "md5.h"
#include "dal.h"
#include "model.h"

void Worker::run()
{
    if (this->m_portList.empty())
        return;
    this->m_socket = new QTcpSocket();
    this->m_socket->setSocketDescriptor(this->m_socketDescriptor);
    if (!this->m_socket->waitForConnected(100000))
    {
        qDebug("IP:%s Connect Fail", m_socket->peerAddress().toString().toStdString().data());
        return;
    }
    std::string ip = m_socket->peerAddress().toString().toStdString();
    qDebug("IP:%s Connect Success, Waiting for verification...", ip.data());

    if (this->m_socket->waitForReadyRead(3000))
    {
        QByteArray data = this->m_socket->readAll();
        qDebug("IP:%s send data:'%s' ---- Verifying password...", ip.data(), data.toStdString().data());

        QString ip = this->m_socket->peerAddress().toString();
        // 校验
//        if (data.toStdString()
//                == MD5("asdfas35.v;cxv-123"
//                     + MD5("xck3dy$^@1309uyrew"
//                           + ip.toStdString()
//                           + "ioer6719024yoiuew6f178934056").toStr()
//                     + "sjavlkc907*$!@(.12i.dy1").toStr())
        if (data.toStdString() == "0d87e84deb68ae435735e0f8ae4668b0")
        {
            qDebug("Verify successful");

            auto list = DAL::instance().getWhiteList(ip);
            for (int port : this->m_portList)
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
                    qDebug("Add to whitelists...");
                    // 添加到白名单中
                    QProcess p(nullptr);
                    p.start("netsh",
                            QStringList() << "ipsec"
                            << "static"
                            << "add"
                            << "filter"
                            << "filterlist=whitelist"
                            << ("srcaddr=" + ip)
                            << "dstaddr=me"
                            << "protocol=tcp"
                            << "mirrored=yes"
                            << QString("dstport=%1").arg(port)
                            );
                    p.waitForStarted();
                    p.waitForFinished();
                }
                else
                {
                    qDebug("Update last login time...");
                }
            }
            if (DAL::instance().updateWhiteList(ip, this->m_portList))
                qDebug("Update completed！");
            else
                qWarning("Update failed!");
        }
        else
        {
            qWarning("Verification failed");
            // 校验失败，若不是算法问题，则可能是其他人想猜密码
        }
    }
    else
    {
        qDebug("Check timeout");
        // 这个连接连上以后不发任何消息，说明不是登录器的socket
        // 登录器的socket会在连接后立刻发送校验数据
        // 所以将这个IP进行记录，当这个IP累计超过一定数量次连接，则将其拉黑
        // TODO:拉黑
    }

    qDebug("Close Socket connection");
    // 关闭socket连接
    this->m_socket->close();
    this->m_socket->deleteLater();
}
