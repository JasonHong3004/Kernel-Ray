#ifndef LATENCYGRAPHVIEW_H
#define LATENCYGRAPHVIEW_H

#include "qcustomplot.h"
#include "cyclicteststatdata.h"
#include <QProcess>
#include <QDebug>


class LatencyGraphView : public QCustomPlot
{
    Q_OBJECT
public:
    LatencyGraphView(QObject *parent);
    QObject *parent;
    QProcess *cyclictestProcess;
    QList <CyclictestStatData*> *cyclictestStatData;

    void runCyclictest(QString command);
    void terminateCyclictest();
    void analyzeRedundantDataSet(QVector<double>*, QVector<double>*, int);

public slots:
    void updateGraphSlot(QList <CyclictestStatData*> *cyclictestStatData, int threadNumber, int deadLineValue);
    void selectionChangeSlot();
    void mousePressSlot();
    void mouseWheelSlot();

};


#endif // LATENCYGRAPHVIEW_H
