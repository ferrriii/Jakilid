#-------------------------------------------------
#
# Project created by QtCreator 2014-05-26T16:23:19
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

LIBS     += -ldataaccess -ldomain

TARGET = tst_profilerepositorytest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_profilerepositorytest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
