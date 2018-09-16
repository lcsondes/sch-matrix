TEMPLATE = app
TARGET = animeditor
INCLUDEPATH += . \
               src \
               src/anim \
               src/gui \
               src/anim/audio \
               src/anim/video \
               src/gui/matrix \
               src/gui/mdi \
               src/gui/palette \
               src/gui/scenetree \
               src/gui/schwindow \
               src/gui/textrender \
               src/gui/tools

INCLUDEPATH += $$matrix_top_dir/ext_libs/lua/src

# Input
HEADERS += src/pch.h \
           src/global.h \
           src/anim/qp4debug.h \
           src/anim/qp4handler.h \
           src/anim/qpaloader.h \
           src/gui/aboutwindow.h \
           src/gui/mainwindow.h \
           src/gui/metaeditdialog.h \
           src/anim/audio/audioplayer.h \
           src/anim/video/clip.h \
           src/anim/video/frame.h \
           src/anim/video/interpolator.h \
           src/anim/video/matrixstack.h \
           src/gui/matrix/backgrounditem.h \
           src/gui/matrix/matrixitem.h \
           src/gui/matrix/pixelitem.h \
           src/gui/matrix/schwidget.h \
           src/gui/mdi/clipmdisubwindow.h \
           src/gui/palette/colorswatch.h \
           src/gui/palette/palette.h \
           src/gui/palette/selectedcolors.h \
           src/gui/scenetree/childsettingsdialog.h \
           src/gui/scenetree/newclipdialog.h \
           src/gui/scenetree/scenetree.h \
           src/gui/scenetree/scenetreepanel.h \
           src/gui/schwindow/playcontrols.h \
           src/gui/schwindow/schwindow.h \
           src/gui/schwindow/timeline.h \
           src/gui/textrender/textrenderdialog.h \
           src/gui/tools/tools.h
SOURCES += src/main.cpp \
           src/anim/qp4debug.cpp \
           src/anim/qp4handler.cpp \
           src/anim/qp4handler_lua.cpp \
           src/anim/qpaloader.cpp \
           src/gui/aboutwindow.cpp \
           src/gui/mainwindow.cpp \
           src/gui/metaeditdialog.cpp \
           src/anim/audio/audioplayer.cpp \
           src/anim/video/clip.cpp \
           src/anim/video/frame.cpp \
           src/anim/video/interpolator.cpp \
           src/anim/video/matrixstack.cpp \
           src/gui/matrix/backgrounditem.cpp \
           src/gui/matrix/matrixitem.cpp \
           src/gui/matrix/pixelitem.cpp \
           src/gui/matrix/schwidget.cpp \
           src/gui/mdi/clipmdisubwindow.cpp \
           src/gui/palette/colorswatch.cpp \
           src/gui/palette/palette.cpp \
           src/gui/palette/selectedcolors.cpp \
           src/gui/scenetree/childsettingsdialog.cpp \
           src/gui/scenetree/newclipdialog.cpp \
           src/gui/scenetree/scenetree.cpp \
           src/gui/scenetree/scenetreepanel.cpp \
           src/gui/schwindow/playcontrols.cpp \
           src/gui/schwindow/schwindow.cpp \
           src/gui/schwindow/timeline.cpp \
           src/gui/textrender/textrenderdialog.cpp \
           src/gui/tools/tools.cpp
RESOURCES += gfx.qrc
TRANSLATIONS += animeditor_hu.ts
LIBS += -L$$matrix_top_dir/ext_libs/lua/src -llua
unix:LIBS += -ldl

QT = core widgets svg network
PRECOMPILED_HEADER = src/pch.h

mtx_static:QTPLUGIN += qsvg

include(../apps.pri)
