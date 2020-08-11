#-------------------------------------------------
#
# Project created by QtCreator 2014-06-25T12:31:01
#
#-------------------------------------------------

QT       += testlib sql

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_databaseservicetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_databaseservicetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
