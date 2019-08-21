#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T12:42:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EgProjectsSample
TEMPLATE = app

#LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_8_0_MinGW_32bit-Debug/debug -lEcoGraphDb

#DEPENDPATH += . ../EcoGraphDb
#INCLUDEPATH += ../EcoGraphDb/include

#win32:CONFIG(release, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_8_0_MinGW_32bit-Release/release -lEcoGraphDb
#else:win32:CONFIG(debug, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_8_0_MinGW_32bit-Debug/debug -lEcoGraphDb

win32:CONFIG(release, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_12_4_MinGW_64_bit-Release/release -lEcoGraphDb
else:win32:CONFIG(debug, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_12_4_MinGW_64_bit-Debug/debug -lEcoGraphDb

#else:unix: LIBS += -L$$PWD/build-MyLibrary/ -lMyLibrary

SOURCES += main.cpp \
    mainwindow.cpp \
    project_form.cpp \
    funcblocks.cpp \
    funcblock_form.cpp \
    projects.cpp \
    references.cpp

HEADERS += mainwindow.h \
    project_form.h \
    funcblocks.h \
    funcblock_form.h \
    projects.h \
    references.h


FORMS += mainwindow.ui \
    project_form.ui \
    funcblocks.ui \
    funcblock_form.ui \
    projects.ui \
    references.ui
