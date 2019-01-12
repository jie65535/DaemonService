#ifndef WORKER_H
#define WORKER_H

#include <QTcpSocket>
#include <QRunnable>
#include <QList>

class Worker : public QRunnable
{
public:
    /**
     * @brief 唯一构造
     * @param socket对象
     */
    Worker(qintptr socketDescriptor, const QList<int> &portList)
        : m_socketDescriptor(socketDescriptor), m_portList(portList){}

protected:
    /**
     * @brief 线程运行方法
     */
    virtual void run();
private:
    /**
     * @brief socket对象
     */
    QTcpSocket *m_socket;
    qintptr m_socketDescriptor;
    QList<int> m_portList;
};

#endif // WORKER_H
