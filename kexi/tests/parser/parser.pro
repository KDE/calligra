TEMPLATE	= app

include( $(KEXI)/common.pro )

win32 {

QMAKE_CXXFLAGS += $(KEXI_OPTIONS)

# test specific:
 LIBS += \
	$$KDELIBDESTDIR/kexidb$$KEXILIB_SUFFIX

#allow to select target independently from debug information
 CONFIG += console
 CONFIG -= windows
}

DESTDIR = . # no # $KDEDIR/bin
TARGET		= parser

SOURCES = \
main.cpp

HEADERS		= 
