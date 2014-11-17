#ifndef CYCLICTESTSTATDATA_H
#define CYCLICTESTSTATDATA_H


#include <QList>
#include <QDebug>


class CyclictestStatData
{
public:
    CyclictestStatData();


    int sum, avg, min, max, missCount, loops, deadline;
    double missRate;
    QList <int> *statDataList;
    bool resetStatData();
    void calculateStatData(int);
    void sortStatData();
    int calcMiss(int);

};

#endif // CYCLICTESTSTATDATA_H
