#ifndef TASKPLOTDIALOG_H
#define TASKPLOTDIALOG_H

#include "data.h"

#include <QPushButton>
#include <QTreeWidget>

class TaskPlotDialog : public QDialog
{
    Q_OBJECT
public:
    TaskPlotDialog(QObject *parent, Data *data, QStringList *checkedTaskList);

    QObject *parent;
    QTreeWidget *taskTreeWidget;

private:
    // Properties
    Data *data;
    QStringList *checkedTaskList;
    QPushButton *okButton;
    QPushButton *cancelButton;

    // Methods
    void recursiveChecks(QTreeWidgetItem *parent);

private slots:
    void updateCheckState(QTreeWidgetItem *item, int column);

};

#endif // TASKPLOTDIALOG_H
