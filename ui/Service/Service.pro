#-------------------------------------------------
#
# Project created by QtCreator 2014-08-23T14:39:45
#
#-------------------------------------------------

QT       += core sql websockets
QT       -= gui

LIBS =  -lapplication -ldataaccess -ldomain


TARGET = JakilidService
CONFIG   -= console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    clientmanager.cpp \
    websocketservice.cpp \
    jsonpservice.cpp \
    httpsocket.cpp

HEADERS += \
    clientmanager.h \
    websocketservice.h \
    jsonpservice.h \
    httpsocket.h

RESOURCES += \
    Resource.qrc

win32:RC_FILE = details.rc

include(../../common.pri)

win32 {
    WINSDK_DIR = C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A
    WIN_PWD = $$replace(PWD, /, \\)
    OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
#    QMAKE_POST_LINK = "$$WINSDK_DIR/bin/x64/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest.xml) -outputresource:$$quote($$OUT_PWD_WIN\\${DESTDIR_TARGET};1)"
   # QMAKE_POST_LINK = "$$PWD/../../tool/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest.xml) -outputresource:$$quote($$OUT_PWD_WIN\\${DESTDIR_TARGET};1)"
}
