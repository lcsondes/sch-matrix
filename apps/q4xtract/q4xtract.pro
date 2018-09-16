TEMPLATE = app
TARGET = 
DEPENDPATH += . src src\addon
INCLUDEPATH += .

# Input
SOURCES += src/main.cpp

QT = core

CONFIG += console

include(../../config.pri)
include(../config.pri)
