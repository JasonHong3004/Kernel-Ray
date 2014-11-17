#ifndef CYCLICTESTOPTIONDATA_H
#define CYCLICTESTOPTIONDATA_H


#include <QLineEdit>
#include <QCheckBox>
#include <QDebug>
#include <QComboBox>



class CyclictestOptionData : public QObject
{
    Q_OBJECT

public:
    CyclictestOptionData();

    QCheckBox *affinityCheckBox;
    QCheckBox *alignedCheckBox;
    QCheckBox *breakTraceCheckBox;
    QCheckBox *clockCheckBox;
    QCheckBox *distanceCheckBox;
    QCheckBox *durationCheckBox;
    QCheckBox *intervalCheckBox;
    QCheckBox *loopCheckBox;
    QCheckBox *priorityCheckBox;
    QCheckBox *threadCreateCheckBox;
    QCheckBox *policyCheckBox;
    QCheckBox *memoryLockCheckBox;
    QCheckBox *nanoSleepCheckBox;
    QCheckBox *sysFuncCheckBox;

    QLineEdit *affinityLineEdit;
    QLineEdit *alignedLineEdit;
    QLineEdit *breakTraceLineEdit;
    QLineEdit *clockLineEdit;
    QLineEdit *distanceLineEdit;
    QLineEdit *durationLineEdit;
    QLineEdit *intervalLineEdit;
    QLineEdit *loopLineEdit;
    QLineEdit *priorityLineEdit;
    QLineEdit *threadCreateLineEdit;

    QComboBox *clockTypeComboBox;
    QComboBox *policyTypeComboBox;

    QList <QCheckBox*> *checkBoxList;
    QList <QLineEdit*> *lineEditList;

    QString getCommand();
    int getThreadNumber();
    int getInfoLineCount();

public slots:
    void changeLineEditEnableSlot();


};

#endif // CYCLICTESTOPTIONDATA_H
