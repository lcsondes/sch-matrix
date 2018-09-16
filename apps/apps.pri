INCLUDEPATH += \
    $$matrix_top_dir/int_libs/mnp4/include \
    $$matrix_top_dir/int_libs/qtiocompressor \
    $$matrix_top_dir/ext_libs/fmodex/api/inc \

LIBS += -L$$matrix_top_dir/dist -lmnp4 -lqtiocompressor

LIBS += -L$$matrix_top_dir/ext_libs/fmodex/api/lib
unix:LIBS += -lfmodex64
win32:LIBS += -lfmodex64_vc
    
QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9

DESTDIR = $$matrix_top_dir/dist/
build_dir = $$matrix_top_dir/build/$${TARGET}
OBJECTS_DIR = $$build_dir/obj
MOC_DIR = $$build_dir/moc
RCC_DIR = $$build_dir/rcc
UI_DIR = $$build_dir/ui