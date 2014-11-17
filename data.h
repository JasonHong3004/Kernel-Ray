#ifndef DATA_H
#define DATA_H

#include "tracethread.h"
#include "ftracedata.h"
#include "cyclictestoptiondata.h"

#include <QRegExp>
#include <QTabWidget>
#include <QList>
#include <QMap>

class Data : public QObject
{
    Q_OBJECT
public:
    Data(QObject *parent);

    QObject *parent;
    TraceThread *traceThread;
    CyclictestOptionData *cyclicOptionData;
    QList <FtraceData *> *ftraceDataList;
    QList <QList<FtraceData *> *> *ftraceDataPerCpuList;
    QMap <QString, QList<FtraceData *> *> *taskHistoryMap;

signals:
    void finishParseSignal();

private slots:
    void parseFtraceSlot(int mode);

private:
    // Properties
    QList <QString *> *tempReadLineListFromFtrace;

    // Methods

};

#endif // DATA_H
