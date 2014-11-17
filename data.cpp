#include "data.h"
#include "tabwidget.h"

Data::Data(QObject *parent)
{
    this->parent = parent;
    this->tempReadLineListFromFtrace = new QList<QString*>();
    this->cyclicOptionData = new CyclictestOptionData();
    // Ordered ftrace data list
    this->ftraceDataList = new QList<FtraceData*>();
    // Ftrace data per CPU list
    this->ftraceDataPerCpuList = new QList<QList<FtraceData *>*>();
    for(int i=0; i<4; i++)
        this->ftraceDataPerCpuList->push_back(new QList<FtraceData *>());
    // Task : Ftrace data map
    this->taskHistoryMap = new QMap<QString, QList<FtraceData *>*>();

    this->traceThread = new TraceThread(this->parent, this->tempReadLineListFromFtrace);
    connect(this->traceThread, SIGNAL(finishReadingFromFtraceSignal(int)), this, SLOT(parseFtraceSlot(int)));
}

/*
 * Exp : Slot for parsing ftrace data to kernel ray
 * Parmeter : View mode(0: TRACE_VIEW, 1: EVENT_VIEW)
 * Return value : None
 * Notice :
 */
void Data::parseFtraceSlot(int mode)
{
    QRegExp delimeter("(\\: |\\ |\\t)");
    QString *function = ((TabWidget*)this->parent)->getStringFromFtrace("current_tracer");

    // Clear all data structures
    this->ftraceDataList->clear();
    for(int i=0; i<4; i++)
        this->ftraceDataPerCpuList->at(i)->clear();
    this->taskHistoryMap->clear();

    if(this->tempReadLineListFromFtrace->size() > 0) {
        for(int i=0; i<this->tempReadLineListFromFtrace->size(); i++) {
            QStringList stringList = this->tempReadLineListFromFtrace->at(i)->split(delimeter, QString::SkipEmptyParts);
            if(stringList.size() >= 4) {
                FtraceData *ftraceData;
                if(!mode && !(function->compare("function")))
                    ftraceData = new FtraceData(stringList, i, 1);
                else
                    ftraceData = new FtraceData(stringList, i, mode);
                this->ftraceDataList->push_back(ftraceData);
                this->ftraceDataPerCpuList->at(ftraceData->getCpu())->push_back(ftraceData);
                if( !(taskHistoryMap->contains( *(ftraceData->getCmd()) ))  )
                    this->taskHistoryMap->insert(*(ftraceData->getCmd()), new QList<FtraceData *>());
                this->taskHistoryMap->value(*(ftraceData->getCmd()))->push_back(ftraceData);
            }
        }

        emit finishParseSignal();

        this->tempReadLineListFromFtrace->clear();
    }
}
