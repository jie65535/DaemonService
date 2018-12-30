#include <QFile>
#include <QTextStream>
#include "log.h"

void Log::append(QString msg)
{
    //保存输出相关信息到指定文件
    QFile outputFile("DaemonServiceLog.txt");
    outputFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&outputFile);
    textStream << msg << endl;

    emit appendEvent(msg);
}
