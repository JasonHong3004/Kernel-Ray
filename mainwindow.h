#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tabwidget.h"

#include <QMainWindow>
#include <QStatusBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    TabWidget *tabWidget;

};

#endif // MAINWINDOW_H
