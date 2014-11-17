#include "settingeventdialog.h"
#include "tabwidget.h"

#include <QRegExp>
#include <QTreeWidgetItem>


SettingEventDialog::SettingEventDialog(QObject *parent)
{
    this->parent = parent;

    // Set eventTreeWidget
    this->eventTreeWidget = new QTreeWidget;
    this->eventTreeWidget->setColumnCount(1);
    this->eventTreeWidget->setHeaderLabel("Event");

    QRegExp delimeter("(\\:)");
    this->availableEventMap = new QMap<QString, QStringList *>();
    this->settedEventMap = new QMap<QString, QStringList *>();

    // Check available events and create event list
    QStringList *availableEventList = ((TabWidget *)this->parent)->getLineStringListFromFtrace("available_events");
    for(int i=0; i<availableEventList->size(); i++) {
        QStringList stringList = availableEventList->at(i).split(delimeter, QString::SkipEmptyParts);
        if( !(this->availableEventMap->contains(stringList.at(0))) )
            this->availableEventMap->insert(stringList.at(0), new QStringList);
        this->availableEventMap->value(stringList.at(0))->push_back(stringList.at(1));
    }

    // Check already setted event and create setted event list
    QStringList *settedEventHash = ((TabWidget *)this->parent)->getLineStringListFromFtrace("set_event");
    for(int i=0; i<settedEventHash->size(); i++) {
        QStringList stringList = settedEventHash->at(i).split(delimeter, QString::SkipEmptyParts);
        if( !(this->settedEventMap->contains(stringList.at(0))) )
            this->settedEventMap->insert(stringList.at(0), new QStringList);
        this->settedEventMap->value(stringList.at(0))->push_back(stringList.at(1));
    }

    // Create event tree widget
    QTreeWidgetItem *allItem = new QTreeWidgetItem(this->eventTreeWidget);
    allItem->setText(0, "All");
    allItem->setCheckState(0, Qt::Unchecked);

    foreach(QString eventName, this->availableEventMap->keys()) {
        QTreeWidgetItem *parentItem = new QTreeWidgetItem(allItem);
        parentItem->setText(0, eventName);
        if(this->settedEventMap->contains(eventName) && (this->availableEventMap->value(eventName)->size() == this->settedEventMap->value(eventName)->size()))
            parentItem->setCheckState(0, Qt::Checked);
        else
            parentItem->setCheckState(0, Qt::Unchecked);

        QStringList *list = this->availableEventMap->value(eventName);
        for(int i=0; i<list->size(); i++) {
            QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
            childItem->setText(0, list->at(i));
            if(this->settedEventMap->contains(eventName) && this->settedEventMap->value(eventName)->contains(list->at(i)))
                childItem->setCheckState(0, Qt::Checked);
            else
                childItem->setCheckState(0, Qt::Unchecked);
        }
    }

    connect(this->eventTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
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
    mainLayout->addWidget(this->eventTreeWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle("Event Option");
    setFixedSize(300, 500);
}

/*
 * Exp : Method for updating check state if user choose checkbox
 * Parmeter : Select check box item, column
 * Return value : None
 * Notice :
 */
void SettingEventDialog::updateCheckState(QTreeWidgetItem *item, int column)
{
    if(!item | column )
        return;

    recursiveChecks(item);
}

/*
 * Exp : Method for updating check state recursivly
 * Parmeter : Select check box item
 * Return value : None
 * Notice :
 */
void SettingEventDialog::recursiveChecks(QTreeWidgetItem *parent)
{
    Qt::CheckState checkState = parent->checkState(0);

    for(int i=0; i<parent->childCount(); i++) {
        parent->child(i)->setCheckState(0, checkState);
        recursiveChecks(parent->child(i));
    }
}

