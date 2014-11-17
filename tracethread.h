#ifndef TRACETHREAD_H
#define TRACETHREAD_H

#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include <QList>
#include <QString>

class TraceThread : public QThread
{
    Q_OBJECT
public:
    // Constructors.
    TraceThread(QObject *parent, QList<QString *> *tempReadLineListFromFtrace);

    QObject *parent;

signals:
    void finishReadingFromFtraceSignal(int mode);

public slots:
    void startReadingSlot();
    void stopReadingSlot();

private:
    // Properties.
    QList<QString *> *tempReadLineListFromFtrace;
    bool isStarted;
    bool isReading;
    int currentMode;

    // Methods.
    void run();
    void readAllFromTrace();
    bool checkModeForStop(QString mode);

};

#endif // TRACEPIPETHREAD_H
