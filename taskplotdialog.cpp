#include "taskplotdialog.h"

#include <QMapIterator>
#include <QTreeWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>

TaskPlotDialog::TaskPlotDialog(QObject *parent, Data *data, QStringList *checkedTaskList)
{
    this->parent = parent;
    this->data = data;
    this->checkedTaskList = checkedTaskList;

    // Set taskTreeWidget
    this->taskTreeWidget = new QTreeWidget;
    this->taskTreeWidget->setColumnCount(1);
    this->taskTreeWidget->setHeaderLabel("Task");

    // Get Task List
    QMap <QString, QList<FtraceData *> *> *taskList = this->data->taskHistoryMap;

    // Create taskTreeWidget
    QMapIterator<QString, QList<FtraceData *> *> mapIter(*taskList);
    QTreeWidgetItem *allTask = new QTreeWidgetItem(this->taskTreeWidget);
    allTask->setText(0, "All Task");
    allTask->setCheckState(0, Qt::Unchecked);

    while(mapIter.hasNext()) {
        mapIter.next();
        QTreeWidgetItem *item = new QTreeWidgetItem(allTask);
        item->setText(0, mapIter.key());
        if(this->checkedTaskList->contains(mapIter.key()))
            item->setCheckState(0, Qt::Checked);
        else
            item->setCheckState(0, Qt::Unchecked);
    }

    connect(this->taskTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updateCheckState(QTreeWidgetItem*,int)));

    // Button & Main Layout
    this->okButton = new QPushButton("OK");
    this->cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(this->okButton);
    buttonLayout->addWidget(this->cancelButton);

    connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(this->taskTreeWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle("Task Plot");
    setFixedSize(300, 500);
}

/*
 * Exp : Method for updating check state if user choose checkbox
 * Parmeter : Select check box item, column
 * Return value : None
 * Notice :
 */
void TaskPlotDialog::updateCheckState(QTreeWidgetItem *item, int column)
{
    if(!item | column)
        return;

    recursiveChecks(item);
}

/*
 * Exp : Method for updating check state recursivly
 * Parmeter : Select check box item
 * Return value : None
 * Notice :
 */
void TaskPlotDialog::recursiveChecks(QTreeWidgetItem *parent)
{
    Qt::CheckState checkState = parent->checkState(0);
    for(int i=0; i<parent->childCount(); i++) {
        parent->child(i)->setCheckState(0, checkState);
        recursiveChecks(parent->child(i));
    }
}
