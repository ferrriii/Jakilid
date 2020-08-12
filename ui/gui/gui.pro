#-------------------------------------------------
#
# Project created by QtCreator 2014-05-18T10:42:50
#
#-------------------------------------------------

QT       += core gui webenginewidgets sql
QT       -= qml quick PrintSupport quickwidgets
CONFIG -= qtquickcompiler # to make js files available in release


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Jakilid
TEMPLATE = app


#LIBS =  -lapplication -ldataaccess -ldomain

SOURCES += main.cpp\
    mainwin.cpp \
    jakilidui.cpp

HEADERS  += \
    mainwin.h \
    jakilidui.h



RESOURCES += \
    Resource.qrc

win32:RC_FILE = details.rc

include(../../common.pri)

win32 {
    WIN_PWD = $$replace(PWD, /, \\)
    OUT_PWD_WIN = $$replace(DESTDIR, /, \\)
    QMAKE_POST_LINK = "$$PWD/../../tool/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest.xml) -outputresource:$$quote($$OUT_PWD_WIN\\$$TARGET).exe"
}
