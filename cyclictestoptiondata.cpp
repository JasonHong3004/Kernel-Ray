#include "cyclictestoptiondata.h"

CyclictestOptionData::CyclictestOptionData()
{
    this->checkBoxList = new QList <QCheckBox*>;
    this->lineEditList = new QList <QLineEdit*>;

    this->affinityCheckBox = new QCheckBox("Affinity");
    this->affinityCheckBox->setToolTip("run thread #N on processor #N, \n if possible with NUM pin all threads to the processor NUM\n");
    this->checkBoxList->push_back(affinityCheckBox);
    this->alignedCheckBox = new QCheckBox("Aligned");
    this->alignedCheckBox->setToolTip("align thread wakeups to a specific offset\n");
    this->checkBoxList->push_back(alignedCheckBox);
    this->breakTraceCheckBox = new QCheckBox("Break Trace");
    this->breakTraceCheckBox->setToolTip("send break trace command when latency > USEC\n");
    this->checkBoxList->push_back(breakTraceCheckBox);
    this->distanceCheckBox = new QCheckBox("Distance");
    this->distanceCheckBox->setToolTip("distance of thread intervals in us default=500\n");
    this->checkBoxList->push_back(distanceCheckBox);
    this->durationCheckBox = new QCheckBox("Duration");
    this->durationCheckBox->setToolTip("specify a length for the test run default is in seconds, but 'm', 'h', or 'd' maybe added\n to modify value to minutes, hours or days\n");
    this->checkBoxList->push_back(durationCheckBox);
    this->intervalCheckBox = new QCheckBox("Interval");
    this->intervalCheckBox->setToolTip("base interval of thread in us default=1000\n");
    this->checkBoxList->push_back(intervalCheckBox);
    this->loopCheckBox = new QCheckBox("Loops");
    this->loopCheckBox->setToolTip("number of loops: default=10000\n");
    this->checkBoxList->push_back(loopCheckBox);
    this->priorityCheckBox = new QCheckBox("Priority");
    this->priorityCheckBox->setToolTip("priority of highest prio thread\n");
    this->checkBoxList->push_back(priorityCheckBox);
    this->threadCreateCheckBox = new QCheckBox("Number of Threads");
    this->threadCreateCheckBox->setToolTip("number of threads: without NUM, threads = max_cpus, without -t default = 1\n");
    this->checkBoxList->push_back(threadCreateCheckBox);
    this->policyCheckBox = new QCheckBox("Policy");
    this->policyCheckBox->setToolTip("policy of realtime thread, POLI may be fifo(default) or rr\n format: --policy=fifo(default) or --policy=rr\n");
    this->checkBoxList->push_back(policyCheckBox);
    this->clockCheckBox = new QCheckBox("Clock Type");
    this->clockCheckBox->setToolTip("select clock 0 = CLOCK_MONOTONIC (default) 1 = CLOCK_REALTIME\n");
    this->checkBoxList->push_back(clockCheckBox);
    this->memoryLockCheckBox = new QCheckBox("Memory Lock");
    this->memoryLockCheckBox->setToolTip("lock current and future memory allocations\n");
    this->checkBoxList->push_back(memoryLockCheckBox);
    this->nanoSleepCheckBox = new QCheckBox("Use Clock_nanosleep");
    this->nanoSleepCheckBox->setToolTip("use clock_nanosleep\n");
    this->checkBoxList->push_back(nanoSleepCheckBox);
    this->sysFuncCheckBox = new QCheckBox("Use sys_nanosleep, \nsys_setitimer");
    this->sysFuncCheckBox->setToolTip("use sys_nanosleep and sys_setitimer\n");
    this->checkBoxList->push_back(sysFuncCheckBox);

    for( int i = 0 ; i < this->checkBoxList->size() ; i++ ){
        this->checkBoxList->at(0)->setChecked(false);
        connect(this->checkBoxList->at(i), SIGNAL(clicked()), this, SLOT(changeLineEditEnableSlot()));
    }

    this->affinityLineEdit = new QLineEdit;
    this->lineEditList->push_back(affinityLineEdit);
    this->alignedLineEdit = new QLineEdit;
    this->lineEditList->push_back(alignedLineEdit);
    this->breakTraceLineEdit = new QLineEdit;
    this->lineEditList->push_back(breakTraceLineEdit);
    this->distanceLineEdit = new QLineEdit;
    this->lineEditList->push_back(distanceLineEdit);
    this->durationLineEdit = new QLineEdit;
    this->lineEditList->push_back(durationLineEdit);
    this->intervalLineEdit = new QLineEdit;
    this->lineEditList->push_back(intervalLineEdit);
    this->loopLineEdit = new QLineEdit;
    this->lineEditList->push_back(loopLineEdit);
    this->priorityLineEdit = new QLineEdit;
    this->lineEditList->push_back(priorityLineEdit);
    this->threadCreateLineEdit = new QLineEdit;
    this->lineEditList->push_back(threadCreateLineEdit);

    for( int i = 0 ; i < this->lineEditList->size() ; i++ ){
        this->lineEditList->at(i)->setEnabled(false);
    }

    this->policyTypeComboBox = new QComboBox;
    this->policyTypeComboBox->addItem("fifo");
    this->policyTypeComboBox->addItem("rr");
    this->policyTypeComboBox->setEnabled(false);

    this->clockTypeComboBox = new QComboBox;
    this->clockTypeComboBox->addItem("MONOTONIC");
    this->clockTypeComboBox->addItem("REALTIME");
    this->clockTypeComboBox->setEnabled(false);

    connect(this->policyCheckBox, SIGNAL(clicked(bool)), this->policyTypeComboBox, SLOT(setEnabled(bool)));
    connect(this->clockCheckBox, SIGNAL(clicked(bool)), this->clockTypeComboBox, SLOT(setEnabled(bool)));
}

QString CyclictestOptionData::getCommand()
{
    QString cyclictestCommand("cyclictest");

    for( int i = 0 ; i < this->checkBoxList->size() ; i++ ){
        if( this->checkBoxList->at(i)->isChecked() )
            switch(i){
            case 0: cyclictestCommand += (" -a " + this->lineEditList->at(i)->text()); continue;
            case 1: cyclictestCommand += (" -A " + this->lineEditList->at(i)->text()); continue;
            case 3: cyclictestCommand += (" -d " + this->lineEditList->at(i)->text()); continue;
            case 4: cyclictestCommand += (" -D " + this->lineEditList->at(i)->text()); continue;
            case 5: cyclictestCommand += (" -i " + this->lineEditList->at(i)->text()); continue;
            case 6: cyclictestCommand += (" -l " + this->lineEditList->at(i)->text()); continue;
            case 7: cyclictestCommand += (" -p " + this->lineEditList->at(i)->text()); continue;
            case 8: cyclictestCommand += (" -t " + this->lineEditList->at(i)->text()); continue;
            case 9: cyclictestCommand += (" --policy=" + this->policyTypeComboBox->currentText()+ " " ); continue;
            case 10: cyclictestCommand += (" -c " + QString::number(this->clockTypeComboBox->currentIndex())); continue;
            case 11: cyclictestCommand += " -m "; continue;
            case 12: cyclictestCommand += " -n "; continue;
            case 13: cyclictestCommand += " -s "; continue;

            }
        else        //if this options are unchecked set default
            switch(i){
            case 3: cyclictestCommand += (" -d 500"); continue;
            case 5: cyclictestCommand += (" -i 1000"); continue;
            case 6: cyclictestCommand += (" -l 10000"); continue;


            }
    }
    cyclictestCommand += " -v -q";

    return cyclictestCommand;
}

int CyclictestOptionData::getThreadNumber()
{
    if(checkBoxList->at(8)->isChecked())
            return this->lineEditList->at(8)->text().toInt();
    else
        return 1;

}

int CyclictestOptionData::getInfoLineCount()
{
    int infoLineCount = 2;
    if(checkBoxList->at(0)->isChecked())
        if(checkBoxList->at(8)->isChecked())
            infoLineCount += 2 * lineEditList->at(8)->text().toInt();
        else
            infoLineCount += 2 * 1;
    else
        if(checkBoxList->at(8)->isChecked())
            infoLineCount += lineEditList->at(8)->text().toInt();
        else
            infoLineCount += 1;


    return infoLineCount;
}

void CyclictestOptionData::changeLineEditEnableSlot()
{
    for( int i = 0 ; i < this->lineEditList->size() ; i++ ){
        if(this->checkBoxList->at(i)->isChecked())
            this->lineEditList->at(i)->setEnabled(true);
        else
            this->lineEditList->at(i)->setEnabled(false);
    }
}



