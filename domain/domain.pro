#-------------------------------------------------
#
# Project created by QtCreator 2014-05-18T10:10:18
#
#-------------------------------------------------

QT       -= gui
QT       += sql core
QT       -= qml quick PrintSupport quickwidgets

TARGET = domain
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    account.cpp \
    info.cpp \
    profile.cpp \
    database.cpp \
    session.cpp

HEADERS += \
    account.h \
    info.h \
    profile.h \
    IVariantConfigurable.h \
    database.h \
    session.h \
    IJsonConfigurable.h




include(../common.pri)
