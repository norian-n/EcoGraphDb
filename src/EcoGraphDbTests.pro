#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T14:53:56
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EcoGraphDbTests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    egMetaInfo.cpp \
    egDataNode.cpp \
    egDataNodesType.cpp \
    egDataClient.cpp \
    egLocalFiles.cpp \
    egDataNodesGUIconnect.cpp \
    egGraphDatabase.cpp \
    egDataNodesLink.cpp \
    testing/mainTests.cpp \
    testing/egIndexesTests.cpp \
    testing/egGraphDatabaseTests.cpp \
    indexes/egIndexesFiles.cpp \
    indexes/egIndexConditions.cpp \
    indexes/egFingers.cpp \
    indexes/egIndexes.cpp

HEADERS += \
    testing/egIndexesTests.h \
    testing/egGraphDatabaseTests.h \
    indexes/egIndexesFiles.h
