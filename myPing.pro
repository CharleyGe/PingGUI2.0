#-------------------------------------------------
#
# Project created by QtCreator 2017-12-10T20:23:19
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lws2_32

TARGET = Ping
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    pinger.cpp \
    showinformaionmessagebox.cpp \
    innerlineedit.cpp \
    addressitem.cpp

HEADERS  += widget.h \
    pinger.h \
    showinformationmessagebox.h \
    innerlineedit.h \
    addressitem.h

RESOURCES += \
    mainicon.qrc
RC_FILE=icon.rc

TRANSLATIONS = Ping_zh_CN.ts

