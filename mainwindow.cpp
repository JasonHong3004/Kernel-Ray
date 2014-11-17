#include "mainwindow.h"

MainWindow::MainWindow()
{
    this->tabWidget = new TabWidget;

    setCentralWidget(this->tabWidget);
    setWindowTitle("KERNEL RAY");
    showMaximized();
}
