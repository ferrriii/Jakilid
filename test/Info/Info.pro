#-------------------------------------------------
#
# Project created by QtCreator 2014-05-26T15:50:46
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

LIBS     += -ldomain

TARGET = tst_infotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_infotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
