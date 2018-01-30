
QT     += core gui network widgets

CONFIG += c++11 console
CONFIG -= app_bundle

QT       += core gui network


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG(release, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_9_2_MinGW_32bit-Release/release -lEcoGraphDb
else:win32:CONFIG(debug, debug|release): LIBS += -L../build-EcoGraphDb-Desktop_Qt_5_9_2_MinGW_32bit-Debug/debug -lEcoGraphDb

SOURCES += \
    main_test.cpp \
    ../egDataClient.cpp \
    ../egMetaInfo.cpp