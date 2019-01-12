#ifndef LOG_H
#define LOG_H

#include <QObject>

class Log : public QObject
{
    Q_OBJECT
public:
    static Log& instance()
    {
        static Log obj;
        return obj;
    }
    void append(QString msg);
signals:
    void appendEvent(QString msg);

private:
    explicit Log(QObject *parent = nullptr)
        : QObject(parent) { }
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
};

#endif // LOG_H
