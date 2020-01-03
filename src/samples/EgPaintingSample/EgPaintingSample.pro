#-------------------------------------------------
#
# Project created by QtCreator 2017-03-21T14:02:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EgPaintingSample
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_8_0_MinGW_32bit-Debug/debug -lEcoGraphDb

#DEPENDPATH += . ../EcoGraphDb
#INCLUDEPATH += ../EcoGraphDb/include

#win32:CONFIG(release, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_8_0_MinGW_32bit-Release/release -lEcoGraphDb
#else:win32:CONFIG(debug, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_8_0_MinGW_32bit-Debug/debug -lEcoGraphDb

win32:CONFIG(release, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_12_4_MinGW_64_bit-Release/release -lEcoGraphDb
else:win32:CONFIG(debug, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_12_4_MinGW_64_bit-Debug/debug -lEcoGraphDb

#win64:CONFIG(release, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_12_4_MinGW_64_bit-Release/release -lEcoGraphDb
#else:win64:CONFIG(debug, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_12_4_MinGW_64_bit-Debug/debug -lEcoGraphDb

#else:unix: LIBS += -L$$PWD/build-MyLibrary/ -lMyLibrary

SOURCES += main.cpp\
        NodeForm.cpp \
        mainwindow.cpp \
        GraphSceneForm.cpp

HEADERS  += mainwindow.h \
        GraphSceneForm.h \
        NodeForm.h

FORMS    += mainwindow.ui \
        GraphSceneForm.ui \
        NodeForm.ui
