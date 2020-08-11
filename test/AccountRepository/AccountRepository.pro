#-------------------------------------------------
#
# Project created by QtCreator 2014-05-27T10:49:29
#
#-------------------------------------------------

QT       += sql testlib

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_accountrepositorytest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_accountrepositorytest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../common.pri)
