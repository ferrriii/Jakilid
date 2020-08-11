#-------------------------------------------------
#
# Project created by QtCreator 2014-08-21T12:39:25
#
#-------------------------------------------------

QT       += testlib websockets sql

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_websocketservertest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


SOURCES += tst_websocketservertest.cpp \
    ../../ui/Service/websocketservice.cpp \
    ../../ui/Service/clientmanager.cpp


DEFINES += SRCDIR=\\\"$$PWD/\\\"


include(../../common.pri)

HEADERS += \
    ../../ui/Service/websocketservice.h \
    ../../ui/Service/clientmanager.h
