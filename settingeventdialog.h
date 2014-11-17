#ifndef SETTINGEVENTDIALOG_H
#define SETTINGEVENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QGroupBox>
#include <QTreeView>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QDirModel>
#include <QMap>
#include <QTreeWidget>

class SettingEventDialog : public QDialog
{
    Q_OBJECT
public:
    SettingEventDialog(QObject *parent);

    QObject *parent;
    QTreeWidget *eventTreeWidget;

private:
    // Properties
    QMap<QString, QStringList *> *settedEventMap;
    QMap<QString, QStringList *> *availableEventMap;
    QPushButton *okButton;
    QPushButton *cancelButton;

    // Methods
    void recursiveChecks(QTreeWidgetItem *parent);

private slots:
    void updateCheckState(QTreeWidgetItem *item, int column);

};

#endif // SETTINGEVENTDIALOG_H
