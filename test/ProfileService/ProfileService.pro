#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T21:28:15
#
#-------------------------------------------------

QT       += testlib sql

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_profileservicetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_profileservicetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
