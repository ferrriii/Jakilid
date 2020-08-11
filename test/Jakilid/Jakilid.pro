#-------------------------------------------------
#
# Project created by QtCreator 2014-06-18T12:04:52
#
#-------------------------------------------------

QT       += sql testlib gui

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_jakilidtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_jakilidtest.cpp \
    ../../ui/gui/jakilid.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
    ../../ui/gui/jakilid.h

include(../../common.pri)
