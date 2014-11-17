#include "cyclicteststatdata.h"


CyclictestStatData::CyclictestStatData()
{
    statDataList = new QList <int>;
    resetStatData();
}

bool CyclictestStatData::resetStatData()
{
    this->sum = 0;
    this->avg = 0;
    this->max = 0;
    this->min = 1000000;
    this->missCount = 0;
    this->missRate  = 0;
    this->statDataList->clear();

    return true;
}

void CyclictestStatData::calculateStatData(int deadline)
{
    sortStatData();

    this->deadline = deadline;
    this->loops = this->statDataList->size();
    this->min = this->statDataList->at(0);
    this->max = this->statDataList->at(this->statDataList->size()-1);

    for( int i = 0 ; i < statDataList->size() ; i++ )
        this->sum += statDataList->at(i);

    if(statDataList->size() != 0)
        this->avg = sum / statDataList->size();

    if( deadline > 0)
        calcMiss(deadline);
}

void CyclictestStatData::sortStatData(){
    qSort(*statDataList);
}

int CyclictestStatData::calcMiss(int deadline)
{
    for( int i = 0 ; i < this->statDataList->size() ; i++ )
        if(deadline < this->statDataList->at(i)){
            this->missCount = statDataList->size()-i;
            break;
        }

    this->missRate = (double)this->missCount / (double)this->statDataList->size();

    return this->missCount;
}










