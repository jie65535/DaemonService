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

    /**
     * @brief 更新白名单用户
     * @param ip地址
     * @param 端口列表
     * @return 是否成功
     */
    bool updateWhiteList(QString ip, QList<int> portList);

    QList<WhiteListItem> getWhiteList(QString ip);

private:
    DAL();
};

#endif // DAL_H
