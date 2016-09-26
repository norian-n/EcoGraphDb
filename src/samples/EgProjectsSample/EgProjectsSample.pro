#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T12:42:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EgProjectsSample
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    project_form.cpp \
    funcblocks.cpp \
    ../../egDataNodesType.cpp \
    ../../egDataNodesGUIconnect.cpp \
    ../../egGraphDatabase.cpp \
    ../../indexes/egIndexConditions.cpp \
    ../../egDataNode.cpp \
    ../../egDataClient.cpp \
    ../../egLocalFiles.cpp \
    ../../egMetaInfo.cpp \
    ../../egDataNodesLink.cpp \
    ../../indexes/egIndexesFiles.cpp \
    ../../indexes/egIndexes.cpp \
    ../../indexes/egFingers.cpp \
    ../../egEntryNodes.cpp \
    funcblock_form.cpp \
    projects.cpp \
    references.cpp


HEADERS += mainwindow.h \
    project_form.h \
    funcblocks.h \
    ../../indexes/egIndexesFiles.h \
    ../../indexes/egIndexConditions.h \
    funcblock_form.h \
    projects.h \
    references.h


FORMS += mainwindow.ui \
    project_form.ui \
    funcblocks.ui \
    funcblock_form.ui \
    projects.ui \
    references.ui
