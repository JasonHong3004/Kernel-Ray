#ifndef SETTINGFTRACEDIALOG_H
#define SETTINGFTRACEDIALOG_H

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

class SettingFtraceDialog : public QDialog
{
    Q_OBJECT
public:
    SettingFtraceDialog(QObject *parent);

    QObject *parent;
    QLineEdit *bufferSizeLineEdit;
    QLineEdit *bufferTotalSizeLineEdit;
    QLineEdit *setFtraceFilterLineEdit;
    QLineEdit *setFtracePidLineEdit;
    QComboBox *currentTracerComboBox;
    QComboBox *filterOptionComboBox;
    QStringList* setFtraceFilterStringList;
    QStringList* setFtraceNotraceStringList;

private slots:
    void addFilterSlot();
    void deleteFilterSlot();
    void deleteAllFilterSlot();

private:
    // Properties
    QSortFilterProxyModel *filterProxyModel;
    QStandardItemModel *filterModel;
    QGroupBox *ftraceStateGroupBox;
    QGroupBox *setFtraceFilterGroupBox;
    QTreeView *setFtraceFilterTreeView;
    QLabel *currentTracerLabel;
    QLabel *bufferSizeLabel;
    QLabel *bufferTotalSizeLabel;
    QLabel *setFtracePidLabel;
    QPushButton *filterAddButton;
    QPushButton *filterDeleteButton;
    QPushButton *filterDeleteAllButton;
    QPushButton *okButton;
    QPushButton *cancelButton;

    // Methods
    void addItemToCurrentTracerComboBox(QComboBox *comboBox);
    QAbstractItemModel* initializeFilterModel();
    void addFilterModel();
    void updateFilterModel();
    QStringList* getWildFilterList(const QString &string);

};

#endif // SETTINGFTRACEDIALOG_H
