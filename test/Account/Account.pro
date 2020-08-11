#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T15:19:39
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_accounttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -ldomain


SOURCES += tst_accounttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
