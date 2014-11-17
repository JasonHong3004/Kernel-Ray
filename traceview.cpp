#include "traceview.h"
#include "tabwidget.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QTreeWidgetItemIterator>
#include <QFutureWatcher>
#include <QTimer>

TraceView::TraceView(QObject *parent, Data *data)
{
    // Initialize properties
    this->parent = parent;
    this->data = data;
    this->checkedTaskList = new QStringList;
    this->filterList = new QList <FtraceData *>();
    this->pageSize = 200;


    // Set selectMode
    //-------------------------------------------------------------------------------------------------

    this->selectModeGroupBox = new QGroupBox("Select Tracing Mode");

    this->ftraceRadioButton = new QRadioButton("Ftrace");
    this->ftraceRadioButton->setChecked(true);
    this->eventTracerRadioButton = new QRadioButton("Event Tracer");

    this->radioButtonGroup = new QButtonGroup;
    this->radioButtonGroup->addButton(this->ftraceRadioButton);
    this->radioButtonGroup->addButton(this->eventTracerRadioButton);
    connect(this->radioButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(changeButtonSlot(int)));

    QHBoxLayout *radioButtonLayout = new QHBoxLayout;
    radioButtonLayout->addWidget(this->ftraceRadioButton);
    radioButtonLayout->addWidget(this->eventTracerRadioButton);

    this->startButton = new QPushButton;
    this->startButton->setText("FTRACE START");
    connect(this->startButton, SIGNAL(clicked()), this->data->traceThread, SLOT(startReadingSlot()));
    connect(this->startButton, SIGNAL(clicked()), this, SLOT(showStartDialogSlot()));

    this->optionButton = new QPushButton;
    this->optionButton->setText("FTRACE OPTION");
    connect(this->optionButton, SIGNAL(clicked()), this, SLOT(setFtraceOptionSlot()));

    this->taskPlotButton = new QPushButton;
    this->taskPlotButton->setText("TASK PLOT");
    this->taskPlotButton->setEnabled(false);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(this->startButton);
    hBoxLayout->addWidget(this->optionButton);
    hBoxLayout->addWidget(this->taskPlotButton);

    QVBoxLayout *selectModeLayout = new QVBoxLayout;
    selectModeLayout->addLayout(radioButtonLayout);
    selectModeLayout->addLayout(hBoxLayout);

    this->selectModeGroupBox->setLayout(selectModeLayout);

    //-------------------------------------------------------------------------------------------------

    // Set Graph View
    //-------------------------------------------------------------------------------------------------

    this->graphView = new GraphView(this->parent, this->data);
    connect(this->data, SIGNAL(finishParseSignal()), this->graphView, SLOT(drawSlot()));
    connect(this, SIGNAL(drawTaskSignal()), this->graphView, SLOT(drawTaskSlot()));

    //-------------------------------------------------------------------------------------------------

    // Set Data Filter
    //-------------------------------------------------------------------------------------------------

    this->pageLabel = new QLabel("Page :");
    this->pageComboBox = new QComboBox;
    this->pageComboBox->setEnabled(false);

    this->cpuLabel = new QLabel("CPU # :");
    this->cpuComboBox = new QComboBox;
    this->cpuComboBox->setEnabled(false);
    this->cpuComboBox->addItems(QStringList() << "All CPU" << "0" << "1" << "2" << "3");

    this->taskLabel = new QLabel("Task :");
    this->taskComboBox = new QComboBox;
    this->taskComboBox->setEnabled(false);
    this->taskComboBox->addItem("All Tasks");

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(this->pageLabel);
    filterLayout->addWidget(this->pageComboBox);
    filterLayout->addWidget(this->cpuLabel);
    filterLayout->addWidget(this->cpuComboBox);
    filterLayout->addWidget(this->taskLabel);
    filterLayout->addWidget(this->taskComboBox);

    //-------------------------------------------------------------------------------------------------

    // Set Data List
    //-------------------------------------------------------------------------------------------------
    this->traceViewModel = new QStandardItemModel;
    this->traceViewModel->setHorizontalHeaderLabels(QStringList() << "#" << "Task" << "PID" << "CPU" << "Latency" << "Time Stamp" << "Function");
    this->traceViewProxyModel = new QSortFilterProxyModel;
    this->traceViewProxyModel->setSourceModel(this->traceViewModel);

    this->traceViewTreeView = new QTreeView;
    this->traceViewTreeView->setRootIsDecorated(false);
    this->traceViewTreeView->setAlternatingRowColors(true);
    this->traceViewTreeView->setModel(this->traceViewProxyModel);
    this->traceViewTreeView->setSortingEnabled(true);

    QVBoxLayout *dataListLayout = new QVBoxLayout;
    dataListLayout->addLayout(filterLayout);
    dataListLayout->addWidget(this->traceViewTreeView);
    QWidget *dataListWidget = new QWidget;
    dataListWidget->setLayout(dataListLayout);

    //-------------------------------------------------------------------------------------------------

    // Splitter & Main

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(this->graphView);
    splitter->addWidget(dataListWidget);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(this->selectModeGroupBox);
    gridLayout->addWidget(splitter);
    setLayout(gridLayout);
}

/*
 * Exp : Slot for updating trace view data after parsing is done
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceView::updateViewDataSlot()
{
    // Set data
    //-------------------------------------------------------------------------------------------------------

    int pageNow = this->data->ftraceDataList->size();
    if(pageNow > this->pageSize)
        pageNow = this->pageSize;

    insertDataToModel(this->data->ftraceDataList, 0, pageNow);

    //-------------------------------------------------------------------------------------------------------

    // Set Task Plot
    if(!(this->taskPlotButton->isEnabled())) {
        this->taskPlotButton->setEnabled(true);
        connect(this->taskPlotButton, SIGNAL(clicked()), this, SLOT(taskPlotSlot()));
    }

    // Set Filtering
    //-------------------------------------------------------------------------------------------------------
    // Set Page
    setPage(0, "All Tasks");
    changePage(this->filterList, 0);

    // Set Cpu filter
    this->cpuComboBox->setEnabled(true);
    connect(this->cpuComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterByCpuSlot(int)));

    this->taskComboBox->clear();
    this->taskComboBox->addItem("All Tasks");
    this->taskComboBox->setEnabled(true);
    this->taskComboBox->addItems(this->data->taskHistoryMap->keys());
    connect(this->taskComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterByTaskSlot(int)));
}

/*
 * Exp : Slot for clicking Ftrace option button and setting ftrace's option
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceView::setFtraceOptionSlot()
{
    // Create settingFtraceDialog
    this->settingFtraceDialog = new SettingFtraceDialog(this->parent);

    // Execute dialog
    if(this->settingFtraceDialog->exec()) {
        // Set buffer size
        bool ok = false;
        this->settingFtraceDialog->bufferSizeLineEdit->text().toInt(&ok);
        if(ok) {
            ((TabWidget *)this->parent)->setStringToFtrace(this->settingFtraceDialog->bufferSizeLineEdit->text(), "buffer_size_kb");
            ok = false;
        } else
            ((TabWidget *)this->parent)->setStringToFtrace("1408", "buffer_size_kb");

        // Set current tracer
        ((TabWidget *)this->parent)->setStringToFtrace(this->settingFtraceDialog->currentTracerComboBox->currentText(), "current_tracer");

        // Set Pid
        this->settingFtraceDialog->setFtracePidLineEdit->text().toInt(&ok);
        if(ok) {
            ((TabWidget *)this->parent)->setStringToFtrace(this->settingFtraceDialog->setFtracePidLineEdit->text(), "set_ftrace_pid");
            ok = false;
        } else
            ((TabWidget *)this->parent)->setStringToFtrace("", "set_ftrace_pid");

        // Set set_ftrace_filter
        ((TabWidget *)this->parent)->setStringListToFtrace(this->settingFtraceDialog->setFtraceFilterStringList, "set_ftrace_filter");

        // Set set_ftrace_notrace
        ((TabWidget *)this->parent)->setStringListToFtrace(this->settingFtraceDialog->setFtraceNotraceStringList, "set_ftrace_notrace");
    }
}

/*
 * Exp : Slot for clicking Event option button and setting Event tracer's option
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceView::setEventOptionSlot()
{
    // Create setting event dialog
    this->settingEventDialog = new SettingEventDialog(this->parent);
    QStringList *checkedList = new QStringList;

    // Execute dialog
    if(this->settingEventDialog->exec()) {
        // Set event
        QTreeWidgetItemIterator it(this->settingEventDialog->eventTreeWidget);
        while(*it) {
            if((*it)->checkState(0) == Qt::Checked && (*it)->childCount() == 0)
                checkedList->push_back((*it)->text(0));
            it++;
        }
        ((TabWidget *)this->parent)->setStringToFtrace("", "set_event");
        ((TabWidget *)this->parent)->setStringListToFtrace(checkedList, "set_event");
    }
}

/*
 * Exp : Method for setting page using cpufilter and taskfilter
 * Parmeter : Selected cpu number, Selected task name
 * Return value : None
 * Notice :
 */
void TraceView::setPage(int cpuFilter, QString taskFilter)
{
    int pageCount = 0;

    QList <FtraceData *> *tempCpuList;
    if(cpuFilter == 0) // ALL CPU
        tempCpuList = this->data->ftraceDataList;
    else // 1:CPU0, 2:CPU1, 3:CPU2, 4:CPU3
        tempCpuList = this->data->ftraceDataPerCpuList->at(cpuFilter-1);

    this->filterList->clear();
    if(!(taskFilter.compare("All Tasks"))) {
        for(int i=0; i<tempCpuList->size(); i++) {
            FtraceData *data = tempCpuList->at(i);
            this->filterList->push_back(data);
        }
    } else {
        for(int i=0; i<tempCpuList->size(); i++) {
            FtraceData *data = tempCpuList->at(i);
            if(!(data->getCmd()->compare(taskFilter)))
                this->filterList->push_back(data);
        }
    }

    if(this->filterList->size() % this->pageSize > 0)
        pageCount += 1;
    pageCount += this->filterList->size() / this->pageSize;

    disconnect(this->pageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changePageSlot(int)));
    while(this->pageComboBox->count() > 0)
        this->pageComboBox->removeItem(0);

    for(int i=0; i<pageCount; i++)
        this->pageComboBox->addItem(QString::number(i));
    this->pageComboBox->setEnabled(true);
    connect(this->pageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changePageSlot(int)));
}

/*
 * Exp : Method for changing page
 * Parmeter : data list, page index
 * Return value : None
 * Notice :
 * 1. 200 line per page
 */
void TraceView::changePage(QList <FtraceData *> *list, int index)
{
    int start, last;
    start = index*this->pageSize;
    last = index*this->pageSize + this->pageSize;

    if(last > list->size())
        last = list->size();

    insertDataToModel(list, start, last);
}

/*
 * Exp : Method for inserting data list to view model
 * Parmeter : Data list, start index, end index
 * Return value : None
 * Notice :
 */
void TraceView::insertDataToModel(QList<FtraceData *> *list, int start, int end)
{
    this->traceViewModel->removeRows(0, this->traceViewModel->rowCount());

    for(int i=start; i<end; i++) {
        this->traceViewModel->insertRow(i-start);
        FtraceData *ftraceData = list->at(i);
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 0), ftraceData->getNum());
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 1), *(ftraceData->getCmd()));
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 2), ftraceData->getPid());
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 3), ftraceData->getCpu());
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 4), *(ftraceData->getLatency()));
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 5), *(ftraceData->getTime()));
        this->traceViewModel->setData(this->traceViewModel->index(i-start, 6), *(ftraceData->getCaller()));
    }

    for(int column=0; column<this->traceViewModel->columnCount(); column++)
        this->traceViewTreeView->resizeColumnToContents(column);
}

/*
 * Exp : Slot for change button label if user choose Ftrace radio button or Event tracer radio button
 * Parmeter : Selected radio button index
 * Return value : None
 * Notice :
 */
void TraceView::changeButtonSlot(int index)
{
    switch(index) {
    case -2:
        this->startButton->setText("FTRACE START");
        disconnect(this->optionButton, SIGNAL(clicked()), this, SLOT(setEventOptionSlot()));
        this->optionButton->setText("FTRACE OPTION");
        connect(this->optionButton, SIGNAL(clicked()), this, SLOT(setFtraceOptionSlot()));

        break;
    case -3:
        this->startButton->setText("EVENT START");
        disconnect(this->optionButton, SIGNAL(clicked()), this, SLOT(setFtraceOptionSlot()));
        this->optionButton->setText("EVENT OPTION");
        connect(this->optionButton, SIGNAL(clicked()), this, SLOT(setEventOptionSlot()));

        break;
    }
}

/*
 * Exp : Slot for change page
 * Parmeter : Page index
 * Return value : None
 * Notice :
 */
void TraceView::changePageSlot(int index)
{
    changePage(this->filterList, index);
}

/*
 * Exp : Slot for filtering data by CPU number
 * Parmeter : CPU number index
 * Return value : None
 * Notice :
 */
void TraceView::filterByCpuSlot(int index)
{
    setPage(index, this->taskComboBox->currentText());
    changePage(this->filterList, 0);
}

/*
 * Exp : Slot for filtering data by task name
 * Parmeter : Task name index but unused
 * Return value : None
 * Notice :
 */
void TraceView::filterByTaskSlot(int index)
{
    Q_UNUSED(index);

    setPage(this->cpuComboBox->currentIndex(), this->taskComboBox->currentText());
    changePage(this->filterList, 0);
}

/*
 * Exp : Slot for showing task plot dialog if user want to show task graphs
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceView::taskPlotSlot()
{
    // Create task plot dialog
    this->taskPlotDialog = new TaskPlotDialog(this->parent, this->data, this->checkedTaskList);

    // Execute dialog
    if(this->taskPlotDialog->exec()) {
        QTreeWidgetItemIterator it(this->taskPlotDialog->taskTreeWidget);
        this->checkedTaskList->clear();

        while(*it) {
            if((*it)->checkState(0) == Qt::Checked && (*it)->childCount() == 0)
                this->checkedTaskList->push_back((*it)->text(0));
            it++;
        }

        emit drawTaskSignal();
    }
}

/*
 * Exp : Slot for showing progress dialog if user click Task plot button
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void TraceView::showStartDialogSlot()
{
    // Clear & Initialize
    this->checkedTaskList->clear();

    // Create Progress Dialog
    this->startDialog = new QProgressDialog;
    this->startDialog->setWindowTitle(this->startButton->text());
    this->startDialog->setCancelButtonText("Recording Stop");
    this->startDialog->setLabelText("Recording...");
    this->startDialog->setRange(0, 0);
    connect(this->startDialog, SIGNAL(canceled()), this->data->traceThread, SLOT(stopReadingSlot()));

    // Execute dialog
    this->startDialog->exec();
}


