#ifndef LATENCYTESTTHREAD_H
#define LATENCYTESTTHREAD_H

#include <QThread>
#include <QProgressDialog>
#include <QProcess>
#include <QTextStream>
#include <QFile>
#include <QDebug>

class LatencyTestThread : public QThread
{
    Q_OBJECT
public:
    LatencyTestThread(QProgressDialog *, QString, QProcess*);


    bool isCyclictestFinished;
    int lineCount;
    int infoLineCount;
    void run();
    QString command;
    QProgressDialog *progressDialog;
    QProcess *cyclictestProcess;


signals:
    void finishCyclictestSignal();
    void terminateCyclictestSignal();



public slots:



};

#endif // LATENCYTESTTHREAD_H
