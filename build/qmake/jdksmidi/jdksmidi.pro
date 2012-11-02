QT       -= core gui

TARGET = jdksmidi
TEMPLATE = lib
CONFIG += staticlib

DEFINES += 

TOP = ../../..

SOURCES += \
    $$TOP/src/*.cpp 

HEADERS += \
    $$TOP/include/jdksmidi/*.h 

INCLUDEPATH += \
    $$TOP/include 

DEPENDPATH +=  \
    $$TOP/include 




