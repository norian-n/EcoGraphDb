#-------------------------------------------------
#
# Project created by QtCreator 2014-11-11T19:21:14
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EcoGraphDb
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    project_form.cpp \
    funcblocks.cpp \
    egDataNode.cpp \
    egMetaInfo.cpp \
    egDataNodesType.cpp \
    egLocalFiles.cpp \
    egDataClient.cpp \
    egDataNodesGUIconnect.cpp \
    egDataNodesLink.cpp \
    egGraphDatabase.cpp

HEADERS += mainwindow.h \
    project_form.h \
    funcblocks.h \
    egCore.h \
    egMetaInfo.h \
    egDataNode.h \
    egDataNodesType.h \
    egLocalFiles.h \
    egDataClient.h \
    egClientServer.h \
    egDataNodesGUIconnect.h \
    egDataNodesLink.h \
    egGraphDatabase.h

FORMS += mainwindow.ui \
    project_form.ui \
    funcblocks.ui
