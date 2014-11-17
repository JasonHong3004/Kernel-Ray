#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "qcustomplot.h"
#include "data.h"
#include "ftracedata.h"

#include <QMap>
#include <QColor>

class GraphView : public QCustomPlot
{
    Q_OBJECT

public:
    GraphView(QObject *parent, Data *data);

    QObject *parent;

public slots:
    void drawTaskSlot();

private slots:
    void drawSlot();
    void selectAllItemsSlot(QCPLegend* legend,QCPAbstractLegendItem* legendItem);

private:
    // Properties
    Data *data;
    QMap<QString, QColor*> *taskColorMap;
    QVector<double> yAxisTicks;
    QVector<QString> yAxisLabels;
    int drawingTaskPosition;
    QList<QColor *> *cpuColorList;
    QMap<QString, QList<QCPAbstractItem *> *> *taskPlottedMap;

    // Method
    QCPItemLine* createLine(double position, int center, QString info, QColor color);
    QCPItemRect* createRect(double left, double right, int center, QString info, QColor color);

};

#endif // GRAPHVIEW_H
