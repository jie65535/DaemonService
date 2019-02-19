#include <QTcpSocket>
#include <QHostAddress>
#include <QProcess>
#include <QString>
#include <string>
#include "worker.h"
#include "md5.h"
#include "dal.h"
#include "model.h"
#include "ipsechelper.h"

void Worker::run()
{
    if (this->m_portList.empty())
        return;
    this->m_socket = new QTcpSocket();
    this->m_socket->setSocketDescriptor(this->m_socketDescriptor);
    QString ip = m_socket->peerAddress().toString();
    if (!this->m_socket->waitForConnected(5000))
    {
        qDebug("IP:%s Connect Fail（该IP连接失败）", ip.toStdString().data());
        return;
    }
    // qDebug("IP:%s Connect Success, Waiting for verification...（该IP连接成功，等待发送校验信息）", ip.data());

    if (this->m_socket->waitForReadyRead(1000))
    {
        QByteArray data = this->m_socket->readAll();
        // qDebug("IP:%s send data:'%s' ---- Verifying password...（正在校验中...）", ip.data(), data.toStdString().data());

        // 校验
//        if (data.toStdString()
//                == MD5("asdfas35.v;cxv-123"
//                     + MD5("xck3dy$^@1309uyrew"
//                           + ip.toStdString()
//                           + "ioer6719024yoiuew6f178934056").toStr()
//                     + "sjavlkc907*$!@(.12i.dy1").toStr())
		QString password = ip + "asdfas35.v;cxv-123ioer6719024yosjavlkc907*$!@(.12i.dy1iuew6f178934056xck3dy$^@1309uyrew";

        if (1 // 不校验了
                || MD5Check(const_cast<char*>(data.toStdString().data()), const_cast<char*>(password.toStdString().data()), password.toStdString().length())
                || data.length() == password.length())
        {
            //qDebug("Verify successful!（校验成功！）");

            auto list = DAL::instance().getWhiteList(ip);


            // 为了防止短时间内多次连接，检查最后一次连接的时间距离现在有多久
            // 如果不超过30秒，则将其从白名单中移除

            for (int i = this->m_portList.length() - 1; i >= 0; --i)
            {
                int port = this->m_portList[i];
                int index;
                for (index = 0; index < list.length(); ++index)
                {
                    if (list[index].Port == port)
                        break;
                }
                if (index == list.length())
                {
                    //qDebug("Add to whitelists...（正在将该IP添加到白名单...）");
                    IpsecHelper::addItemToWhitelist(ip, port);
                }
                else
                {
                    // 如果这个IP已经在白名单了，检查时间间隔
                    // 如果时间大于当前时间，说明是被手动禁止的（当前时间+10年+禁止时间）
                    if (list[index].LastUpdateTime > QDateTime::currentDateTime())
                    {
                        // 如果倒退十年，时间还没到，说明禁止时间未结束，直接结束本次处理
                        if (list[index].LastUpdateTime.addYears(-10) > QDateTime::currentDateTime())
                        {
                            qDebug("IP:%s 已拒绝", ip.toStdString().data());
                            goto end;
                        }
                        else
                        {
                            qDebug("IP:%s port:%d 恢复白名单", ip.toStdString().data(), port);
                            // 否则说明禁止时间已经结束了，可以恢复其白名单了
                            IpsecHelper::addItemToWhitelist(ip, port);
                            DAL::instance().updateWhiteList(list[index].ID, QDateTime::currentDateTime());
                            // 移除这个端口，防止下面再次更新
                            this->m_portList.removeAt(i);
                        }
                    }
                    // 否则判断上一次更新是不是在30秒内
                    // 如果是的话就将它移出白名单，并且设置禁止时间
                    else if (list[index].LastUpdateTime.addSecs(30) > QDateTime::currentDateTime())
                    {
                        qDebug("IP:%s port:%d 移出白名单", ip.toStdString().data(), port);
                        IpsecHelper::removeItemFromWhiteList(ip, port);
                        DAL::instance().updateWhiteList(list[index].ID, QDateTime::currentDateTime().addYears(10).addSecs(30));
                        this->m_portList.removeAt(i);
                    }

                    //qDebug("Update last login time...（检测到该IP已在白名单，更新其最后上线时间...）");
                }
            }
            if (this->m_portList.length() == 0 || DAL::instance().updateWhiteList(ip, this->m_portList))
                qDebug("IP:%s 已连接", ip.toStdString().data());
            else
                qWarning("Update failed!（更新失败）");
        }
        else
        {
            //qWarning("Verification failed!（校验失败！）");
            //qWarning("正在将该IP拉黑...");
            // 校验失败，若不是算法问题，则可能是其他人想猜密码
            IpsecHelper::addItemToBlackList(ip, 8796);
            DAL::instance().addItemToBlackList(ip, 8796);
            qDebug("IP:%s 已拉黑", ip.toStdString().data());
        }
    }
    else
    {
        //qWarning("Check timeout（超过指定时间未发送任何消息，超时！）");
        // 这个连接连上以后不发任何消息，说明不是登录器的socket
        // 登录器的socket会在连接后立刻发送校验数据
        // 所以将这个IP进行记录，当这个IP累计超过一定数量次连接，则将其拉黑
        //qWarning("正在将该IP拉黑...");
        IpsecHelper::addItemToBlackList(ip, 8796);
        DAL::instance().addItemToBlackList(ip, 8796);
        qDebug("IP:%s 已拉黑", ip.toStdString().data());
    }
end:
    // qDebug("Close Socket connection（关闭Socket连接）");
    // 关闭socket连接
    this->m_socket->close();
    this->m_socket->deleteLater();
}
