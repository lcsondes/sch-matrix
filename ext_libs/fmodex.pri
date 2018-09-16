INCLUDEPATH += ../../ext_libs/fmodex/api/inc
LIBS += -L../../ext_libs/fmodex/api/lib -lfmodex
QMAKE_LFLAGS += -Wl,--enable-stdcall-fixup
