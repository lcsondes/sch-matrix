TEMPLATE = app
TARGET = q4xtract
INCLUDEPATH += . src src/addon

# Input
SOURCES += src/main.cpp

QT = core

CONFIG += console

include(../apps.pri)
