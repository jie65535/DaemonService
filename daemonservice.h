#ifndef DAEMONSERVICE_H
#define DAEMONSERVICE_H

#include <QObject>
#include <QtNetwork>
#include <QList>
#include <QDateTime>
#include <QString>

/**
 * @brief 守护服务类，单例实现
 */
class DaemonService : public QTcpServer
{
    Q_OBJECT
public:
    /**
     * @brief 单例对象
     * @return 唯一对象
     */
    static DaemonService& instance()
    {
        static DaemonService obj;
        return obj;
    }

    void setPortList(QList<int> portList)
    {
        this->m_portList = portList;
    }

private:
    explicit DaemonService() = default;
    DaemonService(const DaemonService&) = delete;
    DaemonService& operator=(const DaemonService&) = delete;
protected:
    /**
     * @brief 当有新连接进入时
     */
    virtual void incomingConnection(qintptr socketDescriptor);
private:
    QList<int> m_portList;
};

#endif // DAEMONSERVICE_H
