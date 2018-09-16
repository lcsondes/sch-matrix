#a static buildekbe nem teszek exceptionkezelést, így ezzel lehet detektálni
#egyébként pedig itt be lehet drótozni manuálisan.
exceptions{
	CONFIG+=mtx_dynamic
	DEFINES+=MTX_QT_IS_DYNAMIC
}else{
	CONFIG+=mtx_static
	DEFINES+=MTX_QT_IS_STATIC
}

QMAKE_CXXFLAGS += -std=gnu++0x
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib

# minden pont két mappa mélységben van
INCLUDEPATH += ../../int_libs/qtiocompressor/
LIBS += -L../../build/ # magukat a libeket itt nem kötjük be