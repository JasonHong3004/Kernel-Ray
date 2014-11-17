#ifndef FTRACEDATA_H
#define FTRACEDATA_H

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

class FtraceData : public QObject
{
    Q_OBJECT
public:
    FtraceData(QStringList stringList, int num, int type);

    // Getter Methods
    QString* getCmd();
    int getPid();
    QString* getLatency();
    QString* getTime();
    QString* getCaller();
    int getNum();
    int getCpu();
    QString* getAllInfo();
    QString* getCpuInfo();

private:
    // Properties
    int num;
    int type;
    QString *cmd;
    int pid;
    QString *latency;
    QString *time;
    QString *caller;
    int cpu;
};

#endif // FTRACEDATA_H
