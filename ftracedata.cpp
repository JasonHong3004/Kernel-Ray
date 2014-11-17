#include "ftracedata.h"

FtraceData::FtraceData(QStringList stringList, int num, int type)
{
    this->num = num;
    this->type = type;
    int tokenNumber = 0;
    int index;
    this->cpu = -1;

    // Set data
    // 1. Set cmd
    while((index = stringList.at(tokenNumber).lastIndexOf('-')) < 0)
        tokenNumber++;
    this->cmd = new QString;
    for(int i=0; i<tokenNumber; i++) {
        this->cmd->append(stringList.at(i));
        this->cmd->append(" ");
    }
    this->cmd->append(stringList.at(tokenNumber).left(index));
    // 2. Set pid
    this->pid = stringList.at(tokenNumber).mid(index+1).toInt();
    // 3. Set other
    QString *timeString;
    switch(type) {
    case 0: // TRACE_VIEW
        this->cpu = stringList.at(tokenNumber+1).left(1).toInt();
        this->latency = new QString(stringList.at(tokenNumber+1).mid(1));
        timeString = new QString(stringList.at(tokenNumber+type+2));
        this->time = new QString(timeString->replace("us", ""));
        break;
    case 1: // EVENT_VIEW
        this->cpu = stringList.at(tokenNumber+1).mid(1, stringList.at(tokenNumber+1).size()-2).toInt();
        this->latency = new QString(stringList.at(tokenNumber+2));
        this->time = new QString(stringList.at(tokenNumber+type+2));
        break;
    }

    this->caller = new QString(stringList.at(tokenNumber+type+3));
    this->caller->append(": ");
    for(int i=tokenNumber+type+4; i<stringList.size(); i++) {
        this->caller->append(stringList.at(i));
        if(i<stringList.size()-1)
            this->caller->append(" ");
    }
}

//----------------------------Getter Methods -----------------------------

QString *FtraceData::getCmd()
{
    return this->cmd;
}

int FtraceData::getPid()
{
    return this->pid;
}

QString *FtraceData::getLatency()
{
    return this->latency;
}

QString *FtraceData::getTime()
{
    return this->time;
}

QString *FtraceData::getCaller()
{
    return this->caller;
}

int FtraceData::getNum()
{
    return this->num;
}

int FtraceData::getCpu()
{
    return this->cpu;
}

QString* FtraceData::getAllInfo()
{
    // #
    // TASK
    // PID
    // CPU#
    // TIMESTAMP
    // LATENCY
    // CALLER
    QString *string = new QString;
    string->append(QString::number(this->num));
    string->append("\n");
    string->append(this->cmd);
    string->append("\n");
    string->append(QString::number(this->pid));
    string->append("\n");
    string->append("CPU");
    string->append(QString::number(this->cpu));
    string->append("\n");
    string->append(this->time);
    string->append("\n");
    string->append(this->latency);
    string->append("\n");
    string->append(this->caller);

    return string;
}

QString *FtraceData::getCpuInfo()
{
    // #
    // TASK
    // PID
    // CPU#
    // TIMESTAMP
    QString *string = new QString;
    string->append(QString::number(this->num));
    string->append("\n");
    string->append(this->cmd);
    string->append("\n");
    string->append(QString::number(this->pid));
    string->append("\n");
    string->append("CPU");
    string->append(QString::number(this->cpu));
    string->append("\n");
    string->append(this->time);

    return string;
}

// -------------------------------------------------------------------------------
