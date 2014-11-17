#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include "data.h"
#include "graphview.h"
#include "settingeventdialog.h"
#include "settingftracedialog.h"
#include "taskplotdialog.h"

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QProgressDialog>

class TraceView : public QWidget
{
    Q_OBJECT
public:
    TraceView(QObject *parent, Data *data);

    QObject *parent;
    QStringList *checkedTaskList;

public slots:
    void updateViewDataSlot();
    void setFtraceOptionSlot();
    void setEventOptionSlot();

signals:
    void drawTaskSignal();

private:
    // Properties
    QGroupBox *selectModeGroupBox;
    QLabel *selectModeLabel;
    QRadioButton *ftraceRadioButton;
    QRadioButton *eventTracerRadioButton;
    QButtonGroup *radioButtonGroup;
    QPushButton *startButton;
    QPushButton *optionButton;
    QPushButton *taskPlotButton;
    QStandardItemModel *traceViewModel;
    QSortFilterProxyModel *traceViewProxyModel;
    QTreeView *traceViewTreeView;
    QLabel *pageLabel;
    QComboBox *pageComboBox;
    QLabel *cpuLabel;
    QComboBox *cpuComboBox;
    QLabel *taskLabel;
    QComboBox *taskComboBox;
    Data *data;
    int pageSize;
    GraphView *graphView;
    SettingEventDialog *settingEventDialog;
    SettingFtraceDialog *settingFtraceDialog;
    QList <FtraceData *> *filterList;
    TaskPlotDialog *taskPlotDialog;
    QProgressDialog *startDialog;

    // Methods
    void setPage(int cpuFilter, QString taskFilter);
    void drawTasks();
    void changePage(QList <FtraceData *> *list, int index);
    void clearFtraceOption();
    void clearEventOption();
    void insertDataToModel(QList<FtraceData *> *list, int start, int end);

private slots:
    void changeButtonSlot(int index);
    void changePageSlot(int index);
    void filterByCpuSlot(int index);
    void filterByTaskSlot(int index);
    void taskPlotSlot();
    void showStartDialogSlot();

};

#endif // EVENTVIEW_H
