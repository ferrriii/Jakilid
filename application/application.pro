#-------------------------------------------------
#
# Project created by QtCreator 2014-05-18T10:10:18
#
#-------------------------------------------------

QT       -= gui
QT       += sql
QT       -= qml quick PrintSupport quickwidgets

CONFIG += c++11

TARGET = application
TEMPLATE = lib
CONFIG += staticlib



HEADERS += \
    profileservice.h \
    sessionaccountservice.h \
    sessionservice.h \
    databaseservice.h \
    client.h

SOURCES += \
    profileservice.cpp \
    sessionaccountservice.cpp \
    sessionservice.cpp \
    databaseservice.cpp \
    client.cpp

include(../common.pri)
