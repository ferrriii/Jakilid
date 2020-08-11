#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T21:01:53
#
#-------------------------------------------------

QT       += sql testlib sql

QT       -= gui

TARGET = tst_databasetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS     += -lapplication -ldataaccess -ldomain

SOURCES += tst_databasetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
