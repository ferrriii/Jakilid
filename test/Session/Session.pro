#-------------------------------------------------
#
# Project created by QtCreator 2014-05-25T15:11:36
#
#-------------------------------------------------

QT       += testlib sql

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_sessiontest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_sessiontest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
