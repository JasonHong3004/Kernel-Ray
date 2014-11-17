#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "traceview.h"
#include "latencytestview.h"
#include "data.h"
#include "settingftracedialog.h"

#include <QTabWidget>
#include <QProcess>

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget();

    // Method for ftrace
    void setStringToFtrace(const QString &value, const QString &path);
    void setStringListToFtrace(const QStringList *valueList, const QString &path);
    QString* getStringFromFtrace(const QString &path);
    QStringList* getLineStringListFromFtrace(const QString &path);
    QStringList* getSplitStringListFromFtrace(const QString &path);

private:
    // Properties
    QString *tracePath;
    Data *data;
    TraceView *traceView;
    LatencyTestView *latencyTestView;

    // Methods
    void setupTabs();

};

#endif // TABWIDGET_H
