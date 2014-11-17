#include "tracethread.h"
#include "tabwidget.h"

#include <QProcess>

TraceThread::TraceThread(QObject *parent, QList<QString *> *tempReadLineListFromFtrace)
{
    this->parent = parent;
    this->tempReadLineListFromFtrace = tempReadLineListFromFtrace;
    this->isStarted = false;
    this->isReading = false;
    this->currentMode = -1;
}

/*
 * Exp : Method for running this thread
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceThread::run()
{
    while(1) {
        if(!this->isReading)
            break;
        readAllFromTrace();
    }

    this->isStarted = false;

    emit finishReadingFromFtraceSignal(this->currentMode);
    this->currentMode = -1;
}

/*
 * Exp : Method for reading data from 'trace'
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceThread::readAllFromTrace()
{
    QStringList *traceStringList = ((TabWidget *)this->parent)->getLineStringListFromFtrace("trace");

    for(int i=0; i<traceStringList->size(); i++)
        this->tempReadLineListFromFtrace->push_back( new QString(traceStringList->at(i)) );

    this->isReading = false;
}

/*
 * Exp : Method for checking Trace or Event mode
 * Parmeter : View mode("FTRACE STOP" / "EVENT STOP")
 * Return value : String sender mode
 * Notice :
 */
bool TraceThread::checkModeForStop(QString mode)
{
    if( !(mode.compare("FTRACE STOP")) && this->currentMode == 0 )
        return true;
    else if( !(mode.compare("EVENT STOP")) && this->currentMode == 1 )
        return true;
    else
        return false;
}

/*
 * Exp : Slot for starting reading data from trace_pipe
 * Parmeter : None
 * Return value : None
 * Notice :
 * 1. 'set_event' file check
 */
void TraceThread::startReadingSlot()
{
    QString mode = ((QPushButton *)sender())->text();

    // Clear 'trace' file
    ((TabWidget *)this->parent)->setStringToFtrace("", "trace");

    if(!mode.compare("FTRACE START")) {       
        // Set Mode
        this->currentMode = 0;
        // Clear event
        ((TabWidget *)this->parent)->setStringToFtrace("", "set_event");

        QString *function = ((TabWidget *)this->parent)->getStringFromFtrace("current_tracer");
        if( function->compare("nop") && !(this->isStarted)) {
            ((TabWidget *)this->parent)->setStringToFtrace("1", "tracing_on");
            this->isStarted = true;
        }
    } else {
        // Set Mode
        this->currentMode = 1;
        // Set nop
        ((TabWidget *)this->parent)->setStringToFtrace("nop", "current_tracer");

        QStringList *eventList = ((TabWidget *)this->parent)->getLineStringListFromFtrace("set_event");
        if(!(this->isStarted) && !(eventList->isEmpty()) ) {
            ((TabWidget *)this->parent)->setStringToFtrace("1", "tracing_on");
            this->isStarted = true;
        }
    }
}

/*
 * Exp : Slot for stoping reading data from trace_pipe
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceThread::stopReadingSlot()
{
    if(this->isStarted) {
        ((TabWidget *)this->parent)->setStringToFtrace("0", "tracing_on");
        this->isReading = true;
        this->start();
    }
}
