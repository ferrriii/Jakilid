#-------------------------------------------------
#
# Project created by QtCreator 2014-10-04T10:21:40
#
#-------------------------------------------------

QT       += sql testlib network

QT       -= gui

LIBS     += -lapplication -ldataaccess -ldomain

TARGET = tst_jsonservicetest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


include(../../common.pri)

SOURCES += tst_jsonservicetest.cpp \
    ../../ui/Service/jsonpservice.cpp \
    ../../ui/Service/httpsocket.cpp \
    ../../ui/Service/clientmanager.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../ui/Service/jsonpservice.h \
    ../../ui/Service/httpsocket.h \
    ../../ui/Service/clientmanager.h
