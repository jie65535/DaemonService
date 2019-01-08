#include "ipsechelper.h"
#include <QProcess>
#include <QStringList>

void IpsecHelper::addItemToWhitelist(QString ip, int port)
{
    ExeCmd("add", "whitelist", ip, port);
}

void IpsecHelper::removeItemFromWhiteList(QString ip, int port)
{
    ExeCmd("delete", "whitelist", ip, port);
}

void IpsecHelper::addItemToBlackList(QString ip, int port)
{
    ExeCmd("add", "blacklist", ip, port);
}

void IpsecHelper::removeItemFromBlackList(QString ip, int port)
{
    ExeCmd("delete", "blacklist", ip, port);
}

void IpsecHelper::addItemToBlackList(int port)
{
    ExeCmd("add", "blacklist", "any", port);
}

void IpsecHelper::ExeCmd(QString cmd, QString filterlist, QString srcaddr, int port)
{
    QProcess p(nullptr);
    p.start("netsh",
            QStringList() << "ipsec"
            << "static"
            << cmd
            << "filter"
            << ("filterlist=" + filterlist)
            << ("srcaddr=" + srcaddr)
            << "dstaddr=me"
            << "protocol=tcp"
            << "mirrored=yes"
            << QString("dstport=%1").arg(port)
            );
    p.waitForStarted();
    p.waitForFinished();
}
