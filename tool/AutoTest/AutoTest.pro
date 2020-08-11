#-------------------------------------------------
#
# Project created by QtCreator 2014-05-18T15:00:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    testresultparser.cpp \
    testcase.cpp \
    testlauncher.cpp \
    testlaunchermodel.cpp \
    testmodeitem.cpp

HEADERS  += mainwindow.h \
    testresultparser.h \
    testcase.h \
    testlauncher.h \
    testlaunchermodel.h \
    testmodeitem.h

FORMS    += mainwindow.ui



RESOURCES += \
    resource.qrc

include(../../common.pri)
