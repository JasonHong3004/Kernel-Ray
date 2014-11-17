#include "settingftracedialog.h"
#include "tabwidget.h"

#include <QProcess>
#include <QRegExp>

SettingFtraceDialog::SettingFtraceDialog(QObject *parent)
{ 
    this->parent = parent;

    // --------------- Ftrace State Part ------------------------

    this->ftraceStateGroupBox = new QGroupBox("Ftrace State");

    this->currentTracerLabel = new QLabel("Current Tracer:");
    this->currentTracerComboBox = new QComboBox;
    addItemToCurrentTracerComboBox(this->currentTracerComboBox);

    QHBoxLayout *currentTracerLayout = new QHBoxLayout;
    currentTracerLayout->addWidget(this->currentTracerLabel);
    currentTracerLayout->addWidget(this->currentTracerComboBox);

    this->bufferSizeLabel = new QLabel("Buffer Size:");
    this->bufferSizeLineEdit = new QLineEdit;
    this->bufferSizeLineEdit->setText( *(((TabWidget *)this->parent)->getStringFromFtrace("buffer_size_kb")) );

    this->bufferTotalSizeLabel = new QLabel("Buffer Total Size:");
    this->bufferTotalSizeLineEdit = new QLineEdit;
    this->bufferTotalSizeLineEdit->setText( *(((TabWidget *)this->parent)->getStringFromFtrace("buffer_total_size_kb")) );
    this->bufferTotalSizeLineEdit->setReadOnly(true);

    QHBoxLayout *bufferSizeLayout = new QHBoxLayout;
    bufferSizeLayout->addWidget(this->bufferSizeLabel);
    bufferSizeLayout->addWidget(this->bufferSizeLineEdit);
    bufferSizeLayout->addWidget(this->bufferTotalSizeLabel);
    bufferSizeLayout->addWidget(this->bufferTotalSizeLineEdit);

    this->setFtracePidLabel = new QLabel("Set Ftrace Pid:");
    this->setFtracePidLineEdit = new QLineEdit;
    this->setFtracePidLineEdit->setText( *(((TabWidget *)this->parent)->getStringFromFtrace("set_ftrace_pid")) );

    QHBoxLayout *setFtracePidLayout = new QHBoxLayout;
    setFtracePidLayout->addWidget(this->setFtracePidLabel);
    setFtracePidLayout->addWidget(this->setFtracePidLineEdit);

    QVBoxLayout *ftraceStateLayout = new QVBoxLayout;
    ftraceStateLayout->addLayout(currentTracerLayout);
    ftraceStateLayout->addLayout(bufferSizeLayout);
    ftraceStateLayout->addLayout(setFtracePidLayout);

    this->ftraceStateGroupBox->setLayout(ftraceStateLayout);

    // ------------------------------------------------------------

    // --------------- Set Filter Part ----------------------------

    this->filterProxyModel = new QSortFilterProxyModel;
    this->filterProxyModel->setSourceModel(initializeFilterModel());

    this->setFtraceFilterTreeView = new QTreeView;
    this->setFtraceFilterTreeView->setRootIsDecorated(false);
    this->setFtraceFilterTreeView->setAlternatingRowColors(true);
    this->setFtraceFilterTreeView->setModel(this->filterProxyModel);
    this->setFtraceFilterTreeView->setSortingEnabled(true);
    for(int column=0; column<this->filterProxyModel->columnCount(); column++)
        this->setFtraceFilterTreeView->resizeColumnToContents(column);

    this->setFtraceFilterGroupBox = new QGroupBox("Set Ftrace Filter");

    this->setFtraceFilterLineEdit = new QLineEdit;
    this->setFtraceFilterLineEdit->setPlaceholderText("Enter Function Name");
    this->setFtraceFilterLineEdit->setFixedWidth(250);

    this->filterOptionComboBox = new QComboBox;
    this->filterOptionComboBox->addItem(tr("+"));
    this->filterOptionComboBox->addItem(tr("-"));

    this->filterAddButton = new QPushButton("Add");
    this->filterDeleteButton = new QPushButton("Delete");
    this->filterDeleteAllButton = new QPushButton("Delete All");
    connect(this->filterAddButton, SIGNAL(clicked()), this, SLOT(addFilterSlot()));
    connect(this->filterDeleteButton, SIGNAL(clicked()), this, SLOT(deleteFilterSlot()));
    connect(this->filterDeleteAllButton, SIGNAL(clicked()), this, SLOT(deleteAllFilterSlot()));

    QGridLayout *setFtraceFilterLayout = new QGridLayout;
    setFtraceFilterLayout->addWidget(this->setFtraceFilterTreeView, 0, 0, 1, 6);
    setFtraceFilterLayout->addWidget(this->setFtraceFilterLineEdit, 1, 1);
    setFtraceFilterLayout->addWidget(this->filterOptionComboBox, 1, 2);
    setFtraceFilterLayout->addWidget(this->filterAddButton, 1, 3);
    setFtraceFilterLayout->addWidget(this->filterDeleteButton, 1, 4);
    setFtraceFilterLayout->addWidget(this->filterDeleteAllButton, 1, 5);
    this->setFtraceFilterGroupBox->setLayout(setFtraceFilterLayout);

    // ------------------------------------------------------------

    // --------------- OK/Cancel Button & Main Layout Part --------

    this->okButton = new QPushButton("OK");
    this->cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(this->okButton);
    buttonLayout->addWidget(this->cancelButton);

    connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(this->ftraceStateGroupBox);
    mainLayout->addWidget(this->setFtraceFilterGroupBox);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle("Ftrace Setting");
}

/*
 * Exp : Slot for adding filter
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void SettingFtraceDialog::addFilterSlot()
{
    if(!this->setFtraceFilterLineEdit->text().isEmpty()) {
        if( !(this->filterOptionComboBox->currentText().compare("+")) ) {
            if(this->setFtraceFilterLineEdit->text().contains("*")) {
                QStringList *addStringList = getWildFilterList(this->setFtraceFilterLineEdit->text().simplified());
                for(int i=0; i<addStringList->size(); i++)
                    this->setFtraceFilterStringList->append(addStringList->at(i));
            } else
                this->setFtraceFilterStringList->append(this->setFtraceFilterLineEdit->text().simplified());
        } else {
            if(this->setFtraceFilterLineEdit->text().contains("*")) {
                QStringList *notraceStringList = getWildFilterList(this->setFtraceFilterLineEdit->text().simplified());
                for(int i=0; i<notraceStringList->size(); i++)
                    this->setFtraceNotraceStringList->append(notraceStringList->at(i));
            } else
                this->setFtraceNotraceStringList->append(this->setFtraceFilterLineEdit->text().simplified());
        }

        updateFilterModel();
    }
}

/*
 * Exp : Slot for deleting filter
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void SettingFtraceDialog::deleteFilterSlot()
{
    int index;

    if(!this->setFtraceFilterLineEdit->text().isEmpty()) {
        if( !(this->filterOptionComboBox->currentText().compare("+")) ) {
            index = this->setFtraceFilterStringList->indexOf(this->setFtraceFilterLineEdit->text().simplified());
            this->setFtraceFilterStringList->removeAt(index);
        } else {
            index = this->setFtraceNotraceStringList->indexOf(this->setFtraceFilterLineEdit->text().simplified());
            this->setFtraceNotraceStringList->removeAt(index);
        }

        updateFilterModel();
    }
}

/*
 * Exp : Slot for deleting filter all
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void SettingFtraceDialog::deleteAllFilterSlot()
{
    this->setFtraceFilterStringList->clear();
    this->setFtraceNotraceStringList->clear();

    updateFilterModel();
}

/*
 * Exp : Add available tracers item to current tracer combo box
 * Parmeter : Current tracer combo box
 * Return value : None
 * Notice :
 */
void SettingFtraceDialog::addItemToCurrentTracerComboBox(QComboBox *comboBox)
{
    // Get available tracers list
    QStringList* availableTracersStringList = ((TabWidget *)this->parent)->getSplitStringListFromFtrace("available_tracers");
    // Get current tracer
    QString *currentTracerString = ((TabWidget *)this->parent)->getStringFromFtrace("current_tracer");
    // Set available tracers list to combobox
    for(int i=0; i<availableTracersStringList->size(); i++) {
        QString tracer = availableTracersStringList->at(i);
        if(tracer.compare("function_graph"))
            comboBox->addItem(tracer);
    }
    // Set current tracer to default value
    int index = comboBox->findText(*currentTracerString);
    if(index >= 0)
        comboBox->setCurrentIndex(index);
}

/*
 * Exp : Initialize to filter model
 * Parmeter : None
 * Return value : Filter model
 * Notice :
 * 1. Read 'set_ftrace_filter' and 'set_ftrace_notrace'
 */
QAbstractItemModel* SettingFtraceDialog::initializeFilterModel()
{
    this->filterModel = new QStandardItemModel;
    this->filterModel->setHorizontalHeaderLabels(QStringList() << "#" << "Function Name" << "Option");

    this->setFtraceFilterStringList = ((TabWidget *)this->parent)->getLineStringListFromFtrace("set_ftrace_filter");
    this->setFtraceNotraceStringList = ((TabWidget *)this->parent)->getLineStringListFromFtrace("set_ftrace_notrace");

    addFilterModel();      

    return this->filterModel;
}

/*
 * Exp : Method for adding filter model -> set_ftrace_filter & set_ftrace_ftrace_notrace
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void SettingFtraceDialog::addFilterModel()
{
    int totalCount = this->setFtraceFilterStringList->size() + this->setFtraceNotraceStringList->size();

    for(int i=0; i<this->setFtraceFilterStringList->size(); i++) {
        this->filterModel->insertRow(i);
        this->filterModel->setData(this->filterModel->index(i, 0), i);
        this->filterModel->setData(this->filterModel->index(i, 1), this->setFtraceFilterStringList->at(i));
        this->filterModel->setData(this->filterModel->index(i, 2), "+");
    }

    for(int i=this->setFtraceFilterStringList->size(); i<totalCount; i++) {
        this->filterModel->insertRow(i);
        this->filterModel->setData(this->filterModel->index(i, 0), i);
        this->filterModel->setData(this->filterModel->index(i, 1), this->setFtraceNotraceStringList->at(i - setFtraceFilterStringList->size()));
        this->filterModel->setData(this->filterModel->index(i, 2), "-");
    }
}

/*
 * Exp : Method for resetting filter model -> set_ftrace_filter & set_ftrace_ftrace_notrace
 * Parmeter : None
 * Return value : None
 * Notice :
 */
void SettingFtraceDialog::updateFilterModel()
{
    // Delete all rows
    this->filterModel->removeRows(0, this->filterModel->rowCount());
    addFilterModel();
}

/*
 * Exp :
 * Parmeter : None
 * Return value :
 * Notice :
 * 1. Have to change all path
 */
QStringList *SettingFtraceDialog::getWildFilterList(const QString &string)
{
    QString *path = new QString("/sys/kernel/debug/tracing/available_filter_functions");
    QProcess cat;
    QProcess grep;

    cat.setStandardOutputProcess(&grep);
    cat.start("cat", QStringList() << *path);
    grep.start("grep", QStringList() << string);
    grep.waitForFinished();
    QTextStream output(&grep);

    QStringList *outputStringList = new QStringList;

    while(!output.atEnd())
        outputStringList->append(output.readLine().simplified());

    cat.close();
    grep.close();

    return outputStringList;
}
