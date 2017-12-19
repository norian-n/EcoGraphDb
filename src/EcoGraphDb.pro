#-------------------------------------------------
#
# Project created by QtCreator 2017-02-28T19:38:28
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = EcoGraphDb
TEMPLATE = lib

# CONFIG += staticlib

DEFINES += ECOGRAPHDB_LIBRARY
DEFINES += EG_LIB_BUILD

HEADERS += indexes/egIndexesFiles.h \
    egEntryNodes.h \
    egNamedAttributes.h \
    egNodesLocation.h \\
    ecographdb_global.h \
    egGraphDatabase.h \
    egCore.h

SOURCES += egMetaInfo.cpp \
    egDataNode.cpp \
    egDataNodesType.cpp \
    egDataClient.cpp \
    egLocalFiles.cpp \
    egGraphDatabase.cpp \
    egDataNodesLink.cpp \
    indexes/egIndexesFiles.cpp \
    indexes/egIndexConditions.cpp \
    indexes/egFingers.cpp \
    indexes/egIndexes.cpp \
    egEntryNodes.cpp \
    egNamedAttributes.cpp \
    egNodesLocation.cpp \
    egGUIconnect.cpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
