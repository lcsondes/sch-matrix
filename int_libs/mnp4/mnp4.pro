TEMPLATE = lib
TARGET = mnp4
INCLUDEPATH += . \
               src \
               include/libmnp4 \
               src/core \
               src/network \
               src/packets \
               include/libmnp4/addon \
               include/libmnp4/core \
               include/libmnp4/network \
               include/libmnp4/packets

INCLUDEPATH += $$matrix_top_dir/int_libs/qtiocompressor

# Input
HEADERS += include/libmnp4/global.h \
           include/libmnp4/init.h \
           include/libmnp4/core/mcanimloader.h \
           include/libmnp4/core/mcanimplayer.h \
           include/libmnp4/core/mcconfig.h \
           include/libmnp4/core/mcemitter.h \
           include/libmnp4/core/mceventdispatcher.h \
           include/libmnp4/core/mcreplier.h \
           include/libmnp4/core/mctime.h \
           include/libmnp4/network/mnclient.h \
           include/libmnp4/network/mnqtcpserver.h \
           include/libmnp4/network/mnserver.h \
           include/libmnp4/network/mnworker.h \
           include/libmnp4/packets/mpacket.h \
           include/libmnp4/packets/mpcontrol.h \
           include/libmnp4/packets/mpframe.h \
           include/libmnp4/packets/mpload.h \
           include/libmnp4/packets/mpping.h \
           include/libmnp4/packets/mpstatus.h \
           include/libmnp4/packets/mptime.h
SOURCES += src/init.cpp \
           src/core/mcanimloader.cpp \
           src/core/mcanimplayer.cpp \
           src/core/mcconfig.cpp \
           src/core/mcemitter.cpp \
           src/core/mceventdispatcher.cpp \
           src/core/mcreplier.cpp \
           src/core/mctime.cpp \
           src/core/mctime_unix.cpp \
           src/core/mctime_win.cpp \
           src/network/mnclient.cpp \
           src/network/mnqtcpserver.cpp \
           src/network/mnserver.cpp \
           src/network/mnworker.cpp \
           src/packets/mpacket.cpp \
           src/packets/mpcontrol.cpp \
           src/packets/mpframe.cpp \
           src/packets/mpload.cpp \
           src/packets/mpping.cpp \
           src/packets/mpstatus.cpp \
           src/packets/mptime.cpp \
           include/libmnp4/core/mceventdispatcher_t.cpp

QT = core network
INCLUDEPATH += include
DEFINES += MNP4_LIBRARY
LIBS += -L$$matrix_top_dir/dist -lqtiocompressor

include(../int_libs.pri)
