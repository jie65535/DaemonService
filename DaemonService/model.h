#ifndef MODEL_H
#define MODEL_H
#include <QString>
#include <QDateTime>

struct BlackListItem
{
    BlackListItem(){}
    BlackListItem(int id, QString ip, QDateTime time, QString remarks)
        : ID(id), IP(ip), Time(time), Remarks(remarks) {}
    int         ID;
    QString     IP;
    QDateTime   Time;
    QString     Remarks;
};
struct WhiteListItem
{
    WhiteListItem(){}
    WhiteListItem(int id, QString ip, int port, QDateTime time)
        : ID(id), IP(ip), Port(port), LastUpdateTime(time) {}
    int         ID;
    QString     IP;
    int         Port;
    QDateTime   LastUpdateTime;
};
#endif // MODEL_H
