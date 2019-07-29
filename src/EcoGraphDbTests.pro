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
    egDataNode.cpp \
    egDataNodesType.cpp \
    egDataClient.cpp \
    egLocalFiles.cpp \
    egGraphDatabase.cpp \
    egDataNodesLink.cpp \
    testing/mainTests.cpp \
    testing/egIndexesTests.cpp \
    testing/egGraphDatabaseTests.cpp \
    indexes/egIndexesFiles.cpp \
    indexes/egIndexConditions.cpp \
    indexes/egFingers.cpp \
    indexes/egIndexes.cpp \
    egEntryNodes.cpp \
    testing/egLinksTests.cpp \
    testing/egLocationTests.cpp \
    egNamedAttributes.cpp \
    egNodesLocation.cpp \
    egGUIconnect.cpp \
    testing/egNamedAttributesTests.cpp \
    egExtraInfo.cpp \
    egLogging.cpp

HEADERS += \
    testing/egIndexesTests.h \
    testing/egGraphDatabaseTests.h \
    indexes/egIndexesFiles.h \
    egEntryNodes.h \
    testing/egLinksTests.h \
    testing/egLocationTests.h \
    egNamedAttributes.h \
    egNodesLocation.h \
    testing/egNamedAttributesTests.h \
    egLogging.h
