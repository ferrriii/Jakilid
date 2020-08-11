#-------------------------------------------------
#
# Project created by QtCreator 2014-05-26T17:05:32
#
#-------------------------------------------------

QT       += sql testlib

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_inforepositorytest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_inforepositorytest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
