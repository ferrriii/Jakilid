#-------------------------------------------------
#
# Project created by QtCreator 2014-05-18T10:10:18
#
#-------------------------------------------------

QT       -= gui
QT       += sql
QT       -= qml quick PrintSupport quickwidgets

TARGET = dataaccess
TEMPLATE = lib
CONFIG += staticlib



HEADERS += \
    accountrepository.h \
    inforepository.h \
    profilerepository.h

SOURCES += \
    inforepository.cpp \
    profilerepository.cpp \
    accountrepository.cpp

include(../common.pri)
