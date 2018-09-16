INCLUDEPATH += $$DEPENDPATH
INCLUDEPATH += ../../int_libs/mnp4/include
LIBS+=-lmnp4 -lqtiocompressor
QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9
