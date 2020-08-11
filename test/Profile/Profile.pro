#-------------------------------------------------
#
# Project created by QtCreator 2014-05-25T16:13:23
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

LIBS     += -ldomain

TARGET = tst_profiletest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_profiletest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
