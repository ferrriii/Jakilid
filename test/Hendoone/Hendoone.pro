#-------------------------------------------------
#
# Project created by QtCreator 2014-06-18T12:04:52
#
#-------------------------------------------------

QT       += sql testlib gui

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_hendoonetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_hendoonetest.cpp \
    ../../ui/gui/hendoone.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
    ../../ui/gui/hendoone.h

include(../../common.pri)
