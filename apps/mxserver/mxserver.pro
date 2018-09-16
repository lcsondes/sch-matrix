TEMPLATE = app
TARGET = 
DEPENDPATH += . src
INCLUDEPATH += .

# Input
HEADERS += src/global.h src/merror.h src/muebserver.h src/pch.h
SOURCES += src/main.cpp src/merror.cpp src/muebserver.cpp
PRECOMPILED_HEADER = src/pch.h

QT = core network

CONFIG += console

include(../../config.pri)
include(../config.pri)
