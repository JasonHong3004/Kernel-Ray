#ifndef LATENCYTESTVIEW_H
#define LATENCYTESTVIEW_H

#include "data.h"
#include "latencygraphview.h"
#include "latencytestthread.h"
#include "cyclicteststatdata.h"

#include <QSplitter>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QProcess>
#include <QProgressDialog>
#include <QList>
#include <QDebug>
#include <QComboBox>

class LatencyTestView : public QWidget
{
    Q_OBJECT
public:
    LatencyTestView(QObject *parent, Data *data);

    QList <CyclictestStatData*> *cyclictestStatDataList;
    QVector < QVector < int > > staticDataList;
    QObject *parent;

private:
    // Properties


    QPushButton *startButton;
    QPushButton *resetButton;
    QPushButton *applyButton;
    QLabel *loopLabel;
    QLabel *avgLatencyLabel;
    QLabel *minLatencyLabel;
    QLabel *maxLatencyLabel;
    QLabel *threadLabel;
    QLabel *deadLineLabel;
    QLabel *missCountLabel;
    QLabel *missRateLabel;
    QComboBox *threadSelectComboBox;
    QLineEdit *loopLineEdit;
    QLineEdit *avgLatencyLineEdit;
    QLineEdit *minLatencyLineEdit;
    QLineEdit *maxLatencyLineEdit;
    QLineEdit *deadLineLineEdit;
    QLineEdit *missCountLineEdit;
    QLineEdit *missRateLineEdit;

    QProgressDialog *progressDialog;
    QProcess *cyclictestProcess;
    Data *data;
    LatencyGraphView *latencyGraphView;
    LatencyTestThread *latencyTestThread;

    QList <QCheckBox*> *checkBoxList;
    QList <QLineEdit*> *lineEditList;

    int lineCount;
    int infoLineCount;
    int total_loop, total_sum, total_avg, total_min, total_max, total_missCount;
    double total_missRate;


    // Methods

    void initView();
    QProgressDialog* setProgressDialog();
    LatencyTestThread* setLatencyTestThread();
    bool readOutputFile();
    void setOptionPart(QGridLayout*);
    void setMainLayout();


private slots:
    void drawGraph(QList <CyclictestStatData*> *, int);
    void startButtonSlot();
    void resetButtonSlot();
    void calculateOutputFileSlot();
    void terminateCyclictestSlot();
    void setSelectedThreadStatData(int);

signals:
    void finishCalculateOutputSignal();



};

#endif // LATENCYTESTVIEW_H
