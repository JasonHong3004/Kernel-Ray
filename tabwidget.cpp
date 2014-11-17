#include "tabwidget.h"

TabWidget::TabWidget()
{
    this->data = new Data(this);

    this->tracePath = new QString("/sys/kernel/debug/tracing/");

    // Initialize Part
    // Set 0 to tracing_on for tracing stop
    setStringToFtrace("0", "tracing_on");
    // Set nop to current_ftrace
    setStringToFtrace("nop", "current_tracer");
    // Clear all events
    setStringToFtrace("", "set_event");
    // Clear set_ftrace_filter
    setStringToFtrace("", "set_ftrace_pid");
    // Clear set_ftrace_filter
    setStringToFtrace("", "set_ftrace_filter");
    // Clear set_ftrace_notrace
    setStringToFtrace("", "set_ftrace_notrace");

    setupTabs();
}

/*
 * Exp : Method for adding view to tab
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TabWidget::setupTabs()
{
    // Add Trace View
    this->traceView = new TraceView(this, this->data);
    connect(this->data, SIGNAL(finishParseSignal()), this->traceView, SLOT(updateViewDataSlot()));
    addTab(this->traceView, "Trace View");

    // Add Latency View
    this->latencyTestView = new LatencyTestView(this, this->data);
    addTab(this->latencyTestView, "Latency View");
}

/*
 * Exp : Method for setting value to path
 * Parmeter : String value and path file name
 * Return value : None
 * Notice :
 */
void TabWidget::setStringToFtrace(const QString &value, const QString &path)
{
    QString *string = new QString(*(this->tracePath));
    string->append(path);

    QProcess echo;
    echo.setStandardOutputFile(*string);
    echo.start("echo", QStringList() << value);
    echo.waitForFinished();
    echo.close();
}

/*
 * Exp : Method for setting value list to path
 * Parmeter : String value list and path file name
 * Return value : None
 * Notice :
 */
void TabWidget::setStringListToFtrace(const QStringList *valueList, const QString &path)
{
    QString *string = new QString(*(this->tracePath));
    string->append(path);

    QProcess echo;
    echo.setStandardOutputFile(*string, QIODevice::Append);

    echo.start("echo", *valueList);
    echo.waitForFinished();

    echo.close();
}

/*
 * Exp : Method for getting value string from path
 * Parmeter : None
 * Return value : Value string
 * Notice :
 */
QString *TabWidget::getStringFromFtrace(const QString &path)
{
    QString *string = new QString(*(this->tracePath));
    string->append(path);
    QProcess cat;

    cat.start("cat", QStringList() << *string);
    cat.waitForFinished();
    QByteArray output = cat.readAll();
    QString valueString(output);
    cat.close();

    return new QString(valueString.simplified());
}

/*
 * Exp : Method for getting split value string list from path
 * Parmeter : None
 * Return value : Value string list
 * Notice :
 */
QStringList *TabWidget::getLineStringListFromFtrace(const QString &path)
{
    QString *string = new QString(*(this->tracePath));
    string->append(path);
    QProcess cat;

    cat.start("cat", QStringList() << *string);
    cat.waitForFinished();
    QTextStream output(&cat);

    QStringList *outputStringList = new QStringList;

    while(!output.atEnd()) {
        QString line = output.readLine();
        if(!line.startsWith('#'))
            outputStringList->append(line.simplified());
    }
    cat.close();

    return outputStringList;
}

/*
 * Exp : Method for getting split value string list from path
 * Parmeter : None
 * Return value : Value string list
 * Notice :
 */
QStringList *TabWidget::getSplitStringListFromFtrace(const QString &path)
{
    QString *string = new QString(*(this->tracePath));
    string->append(path);
    QProcess cat;
    QRegExp delimeter("(\\ |\\t)");

    cat.start("cat", QStringList() << *string);
    cat.waitForFinished();
    QTextStream output(&cat);

    QStringList *outputStringList = new QStringList;

    while(!output.atEnd()) {
        QString line = output.readLine();
        if(!line.startsWith('#')) {
            QStringList* lineStringList = new QStringList(line.simplified().split(delimeter, QString::SkipEmptyParts));
            for (int i=0; i<lineStringList->size(); i++)
                outputStringList->append(lineStringList->at(i).simplified());
        }
    }
    cat.close();

    return outputStringList;
}
