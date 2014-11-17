#-------------------------------------------------
#
# Project created by QtCreator 2014-09-21T15:38:34
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KernelRay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cyclictestoptiondata.cpp \
    cyclicteststatdata.cpp \
    data.cpp \
    ftracedata.cpp \
    latencygraphview.cpp \
    latencytestthread.cpp \
    latencytestview.cpp \
    qcustomplot.cpp \
    settingeventdialog.cpp \
    settingftracedialog.cpp \
    tabwidget.cpp \
    taskplotdialog.cpp \
    tracethread.cpp \
    graphview.cpp \
    traceview.cpp

HEADERS  += mainwindow.h \
    cyclictestoptiondata.h \
    cyclicteststatdata.h \
    data.h \
    ftracedata.h \
    latencygraphview.h \
    latencytestthread.h \
    latencytestview.h \
    qcustomplot.h \
    settingeventdialog.h \
    settingftracedialog.h \
    tabwidget.h \
    taskplotdialog.h \
    tracethread.h \
    graphview.h \
    traceview.h
