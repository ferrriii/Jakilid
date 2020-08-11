#-------------------------------------------------
#
# Project created by QtCreator 2014-05-30T11:56:05
#
#-------------------------------------------------

QT       += sql testlib

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_sessionaccountservicetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_sessionaccountservicetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
