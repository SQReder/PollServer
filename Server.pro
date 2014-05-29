#-------------------------------------------------
#
# Project created by QtCreator 2013-04-24T03:57:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        dialog.cpp \
    pollsserver.cpp \
    pollsclient.cpp \
    protocol.cpp \
    configmanager.cpp

HEADERS  += dialog.h \
    pollsserver.h \
    pollsclient.h \
    protocol.h \
    ui_dialog.h \
    configmanager.h

FORMS    += dialog.ui

OTHER_FILES += \
    server.config
