#ifndef IPSECHELPER_H
#define IPSECHELPER_H

#include <QString>

class IpsecHelper
{
public:
    IpsecHelper() = delete;
    IpsecHelper(IpsecHelper&) = delete;
    IpsecHelper& operator=(const IpsecHelper&) = delete;


    static void addItemToWhitelist(QString ip, int port);
    static void removeItemFromWhiteList(QString ip, int port);

    static void addItemToBlackList(QString ip, int port);
    static void addItemToBlackList(QString ip);
    static void removeItemFromBlackList(QString ip, int port);
    static void removeItemFromBlackList(QString ip);

    static void addItemToBlackList(int port);
private:
    static void ExeCmd(QString cmd, QString filterlist, QString srcaddr, int port);
    static void ExeCmd(QString cmd, QString filterlist, QString srcaddr);
};

#endif // IPSECHELPER_H
