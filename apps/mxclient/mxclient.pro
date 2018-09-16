TEMPLATE = app
TARGET = mxclient
INCLUDEPATH += . \
               src \
               src/core \
               src/gui \
               src/gui/mainwindowparts \
               src/gui/playcontrols \
               src/gui/safebutton \
               src/gui/serverpane \
               src/gui/timeline

# Input
HEADERS += src/global.h \
           src/pch.h \
           src/core/player.h \
           src/core/transmitter.h \
           src/gui/mainwindow.h \
           src/gui/mainwindowparts/dirview.h \
           src/gui/mainwindowparts/playlist.h \
           src/gui/mainwindowparts/playlistmodel.h \
           src/gui/playcontrols/playcontrols.h \
           src/gui/safebutton/safebutton.h \
           src/gui/serverpane/serverpane.h \
           src/gui/timeline/timeline.h
SOURCES += src/main.cpp \
           src/core/player.cpp \
           src/core/transmitter.cpp \
           src/gui/mainwindow.cpp \
           src/gui/mainwindowparts/dirview.cpp \
           src/gui/mainwindowparts/playlist.cpp \
           src/gui/mainwindowparts/playlistmodel.cpp \
           src/gui/playcontrols/playcontrols.cpp \
           src/gui/safebutton/safebutton.cpp \
           src/gui/serverpane/serverpane.cpp \
           src/gui/timeline/timeline.cpp
RESOURCES += mxclient.qrc
PRECOMPILED_HEADER = src/pch.h

QT = core widgets svg network

mtx_static:QTPLUGIN += qsvg

include(../apps.pri)
