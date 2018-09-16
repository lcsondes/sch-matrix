TEMPLATE = lib
TARGET = qtiocompressor
DEPENDPATH += .
INCLUDEPATH += .

INCLUDEPATH += $$[QT_INSTALL_PREFIX]/include/QtZlib

# Input
HEADERS += qtiocompressor.h
SOURCES += qtiocompressor.cpp

CONFIG += staticlib

include(../int_libs.pri)