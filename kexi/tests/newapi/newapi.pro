TEMPLATE	= app

include( $(KEXI)/common.pro )

unix {
 LIBS += -lkexidb
}

win32 {

 LIBS += \
  $$QKWLIB/kexicore$$KEXILIB_SUFFIX \
  $$QKWLIB/kexidatatable$$KEXILIB_SUFFIX \
  $$QKWLIB/kexiextendedwidgets$$KEXILIB_SUFFIX

QMAKE_CXXFLAGS += $(KEXI_OPTIONS)

# test specific:
 LIBS += \
	$$QKWLIB/kexidb$$KEXILIB_SUFFIX

#allow to select target independently from debug information
 CONFIG += console
 CONFIG -= windows
}

DESTDIR = . # no # $KDEDIR/bin
TARGET		= newapi

SOURCES = \
main.cpp

HEADERS		= 
