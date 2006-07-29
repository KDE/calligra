TEMPLATE	= app

include( $(KEXI)/common.pro )

unix {
 LIBS += -lkexidb
}

win32 {

 LIBS += \
  $$KDELIBDESTDIR/kexicore$$KEXILIB_SUFFIX \
  $$KDELIBDESTDIR/kexidatatable$$KEXILIB_SUFFIX \
  $$KDELIBDESTDIR/kexiextendedwidgets$$KEXILIB_SUFFIX

QMAKE_CXXFLAGS += $(KEXI_OPTIONS)

# test specific:
 LIBS += \
	$$KDELIBDESTDIR/kexidb$$KEXILIB_SUFFIX

#allow to select target independently from debug information
 CONFIG += console
 CONFIG -= windows
}

DESTDIR = . # no # $KDEDIR/bin
TARGET		= kexidbtest

SOURCES = \
main.cpp

HEADERS		= 
