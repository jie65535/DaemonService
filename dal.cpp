#include "dal.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QList>
#include "model.h"

bool DAL::updateWhiteList(QString ip, QList<int> portList)
{
    if (portList.empty())
        return false;
    // 先找找看指定项存不存在
    QString sql = QString("SELECT * FROM whitelist WHERE IP=? AND Port in (%1").arg(portList[0]);
    for (int i = 1; i < portList.size(); ++i)
        sql += QString(",%1").arg(portList[i]);
    sql += ");";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(ip);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        return false;
    }
    // 加入列表
    QList<WhiteListItem> whitelist;
    while(query.next())
        whitelist.append(WhiteListItem(query.value(0).toInt(), query.value(1).toString(), query.value(2).toInt(), query.value(3).toDateTime()));

    // 开始事务
    QSqlDatabase database = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    database.transaction();

    // 检查数据库中的白名单
    for (int i = 0; i < whitelist.size(); ++i)
    {
        // 如果要更新的已经存在，则update
        if (portList.contains(whitelist[i].Port))
        {
            query.prepare("UPDATE whitelist SET LastUpdateTime = datetime(CURRENT_TIMESTAMP,'localtime') WHERE IP=? AND Port=?");
            query.addBindValue(ip);
            query.addBindValue(whitelist[i].Port);
            if(!query.exec())
            {
                qCritical()<<query.lastError();
                return false;
            }
            // 将其从whilelist中移出
            portList.removeOne(whitelist[i].Port);
        }
    }
    // 将数据库中不存在的插入到数据库中
    for (int i = 0; i < portList.size(); ++i)
    {
        query.prepare("INSERT INTO whitelist(IP, Port, LastUpdateTime) VALUES(?, ?, datetime(CURRENT_TIMESTAMP,'localtime'))");
        query.addBindValue(ip);
        query.addBindValue(portList[i]);
        if(!query.exec())
        {
            qCritical()<<query.lastError();
            return false;
        }
    }

    // 提交事务
    if (!database.commit())
        qCritical()<<database.lastError();

    return true;
}

QList<WhiteListItem> DAL::getWhiteList(QString ip)
{
    QList<WhiteListItem> whitelist;

    QString sql = QString("SELECT * FROM whitelist WHERE IP=?");
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(ip);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        return whitelist;
    }
    while(query.next())
        whitelist.append(WhiteListItem(query.value(0).toInt(), query.value(1).toString(), query.value(2).toInt(), query.value(3).toDateTime()));
    return whitelist;
}

void DAL::removeFromWhiteList(QString ip, QList<int> ports)
{
    QString portStr = QString("%1").arg(ports[0]);
    for (int i = 0; i < ports.length(); ++i)
        portStr += QString(",%1").arg(ports[i]);
    QString sql = QString("DELETE FROM whitelist WHERE IP=? AND Port in (%1)").arg(portStr);
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(ip);
    if(!query.exec())
        qCritical()<<query.lastError();
}

bool DAL::isExistsBlackList(int port)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM blacklist WHERE IP='any' AND Remarks = ?");
    query.addBindValue(port);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        return false;
    }
    return query.next();
}
bool DAL::isExistsBlackList(QString ip)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM blacklist WHERE IP=?");
    query.addBindValue(ip);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        return false;
    }
    return query.next();
}

bool DAL::addItemToBlackList(int port)
{
    return addItemToBlackList("any", port);
}

bool DAL::addItemToBlackList(QString ip, int port)
{
    QSqlQuery query;
    query.prepare("INSERT INTO blacklist(IP, Time, Remarks) VALUES(?, datetime(CURRENT_TIMESTAMP,'localtime'), ?)");
    query.addBindValue(ip);
    query.addBindValue(port);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        return false;
    }
    return true;
}

QString DAL::getPortList()
{
    QSqlQuery query;
    if(!query.exec("SELECT * FROM portlist"))
    {
        qCritical()<<query.lastError();
        return "";
    }
    if (query.next())
        return query.value(0).toString();
    else
        return "";
}

void DAL::setPortList(QString portList)
{
    QSqlQuery query;
    query.prepare("UPDATE portlist SET value=?");
    query.addBindValue(portList);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
    }
}

DAL::DAL()
{
    //打印Qt支持的数据库驱动
    //qDebug()<<QSqlDatabase::drivers();

    QSqlDatabase database;
    // 检测默认连接是否已经存在
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection))
    {
        // 存在就直接用
        database = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    }
    else
    {
        // 不存在就添加一个数据库驱动引擎SQLite
        database = QSqlDatabase::addDatabase("QSQLITE");
        // 然后打开指定用户的数据库文件
        database.setDatabaseName("data.db");
        // database.setUserName("root");
        // database.setPassword("123456");
    }

    //打开数据库
    if(!database.open())
    {
        // 如果打开失败 退出程序
        qCritical()<<database.lastError();
        qFatal("Database open failed.（数据库打开失败！错误信息已经输出到日志）");
    }
    else
    {
        qDebug("Open database success!（数据库打开成功！）");
        QStringList tables = database.tables();  //获取数据库中的表
        //qDebug() << QString("tablas count： %1").arg(tables.count()); //打印表的个数

        if (tables.count() < 4)
        {
/*
  SQL语句：
CREATE TABLE whitelist (
    ID             INTEGER  PRIMARY KEY AUTOINCREMENT,
    IP             VARCHAR,
    Port           INT,
    LastUpdateTime DATETIME
);
CREATE TABLE blacklist (
    ID      INTEGER  PRIMARY KEY AUTOINCREMENT,
    IP      VARCHAR,
    Time    DATETIME,
    Remarks TEXT
);
*/
            database.transaction();
            QSqlQuery query;
            // TODO:建表
            query.exec("CREATE TABLE whitelist ("
                       "ID INTEGER  PRIMARY KEY AUTOINCREMENT,"
                       "IP VARCHAR,"
                       "Port INT,"
                       "LastUpdateTime DATETIME);");
            query.exec("CREATE TABLE blacklist ("
                       "ID INTEGER  PRIMARY KEY AUTOINCREMENT,"
                       "IP VARCHAR,"
                       "Time DATETIME,"
                       "Remarks TEXT);");
            query.exec("CREATE TABLE portlist (value TEXT);");
            query.exec("INSERT INTO portlist VALUES('7001');");
            if (!database.commit())
                qCritical()<<database.lastError();
        }
    }
}

bool DAL::updateWhiteList(int id, QDateTime lastUpdateTime)
{
    QSqlQuery query;
    query.prepare("UPDATE whitelist SET LastUpdateTime=? WHERE ID=?");
    query.addBindValue(lastUpdateTime);
    query.addBindValue(id);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        return false;
    }
    return true;
}
