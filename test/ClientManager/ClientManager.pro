#-------------------------------------------------
#
# Project created by QtCreator 2014-10-02T22:58:47
#
#-------------------------------------------------

QT       += testlib sql

QT       -= gui
LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_clientmanagertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_clientmanagertest.cpp \
    ../../ui/Service/clientmanager.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"


include(../../common.pri)

HEADERS += \
    ../../ui/Service/clientmanager.h
