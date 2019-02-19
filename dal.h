#ifndef DAL_H
#define DAL_H

#include <QList>
#include <QString>
#include "model.h"

class DAL
{
public:
    static DAL &instance()
    {
        static DAL obj;
        return obj;
    }

    bool updateWhiteList(int id, QDateTime lastUpdateTime);
    /**
     * @brief 更新白名单用户
     * @param ip地址
     * @param 端口列表
     * @return 是否成功
     */
    bool updateWhiteList(QString ip, QList<int> portList);

    QList<WhiteListItem> getWhiteList(QString ip);
    void removeFromWhiteList(QString ip);
    void removeFromWhiteList(QString ip, QList<int> ports);

    bool isExistsBlackList(int port);
    bool isExistsBlackList(QString ip);

    bool addItemToBlackList(int port);
    bool addItemToBlackList(QString ip);
    bool addItemToBlackList(QString ip, int port);
    void removeFromBlackList(QString ip);

    QString getPortList();
    void setPortList(QString portList);
private:
    DAL();
};

#endif // DAL_H
