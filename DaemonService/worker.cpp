#include <QTcpSocket>
#include <QHostAddress>
#include <QProcess>
#include <QString>
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
        qDebug("IP:%s Connect Fail（该IP连接失败）", m_socket->peerAddress().toString().toStdString().data());
        return;
    }
    std::string ip = m_socket->peerAddress().toString().toStdString();
    qDebug("IP:%s Connect Success, Waiting for verification...（该IP连接成功，等待发送校验信息）", ip.data());

    if (this->m_socket->waitForReadyRead(3000))
    {
        QByteArray data = this->m_socket->readAll();
        qDebug("IP:%s send data:'%s' ---- Verifying password...（正在校验中...）", ip.data(), data.toStdString().data());

        QString ip = this->m_socket->peerAddress().toString();
        // 校验
//        if (data.toStdString()
//                == MD5("asdfas35.v;cxv-123"
//                     + MD5("xck3dy$^@1309uyrew"
//                           + ip.toStdString()
//                           + "ioer6719024yoiuew6f178934056").toStr()
//                     + "sjavlkc907*$!@(.12i.dy1").toStr())
		QString password = ip + "asdfas35.v;cxv-123ioer6719024yosjavlkc907*$!@(.12i.dy1iuew6f178934056xck3dy$^@1309uyrew";

        if (MD5Check(const_cast<char*>(data.toStdString().data()), const_cast<char*>(password.toStdString().data()), password.toStdString().length()))
        {
            qDebug("Verify successful!（校验成功！）");

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
                    qDebug("Add to whitelists...（正在将该IP添加到白名单...）");
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
                    qDebug("Update last login time...（检测到该IP已在白名单，更新其最后上线时间...）");
                }
            }
            if (DAL::instance().updateWhiteList(ip, this->m_portList))
                qDebug("Update completed!（更新完成！）");
            else
                qWarning("Update failed!（更新失败）");
        }
        else
        {
            qWarning("Verification failed!（校验失败！）");
            // 校验失败，若不是算法问题，则可能是其他人想猜密码
        }
    }
    else
    {
        qWarning("Check timeout（超过指定时间未发送任何消息，超时！）");
        // 这个连接连上以后不发任何消息，说明不是登录器的socket
        // 登录器的socket会在连接后立刻发送校验数据
        // 所以将这个IP进行记录，当这个IP累计超过一定数量次连接，则将其拉黑
        // TODO:拉黑
    }

    qDebug("Close Socket connection（关闭Socket连接）");
    // 关闭socket连接
    this->m_socket->close();
    this->m_socket->deleteLater();
}
