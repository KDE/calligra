TEMPLATE	= app

include( $(KEXI)/common.pro )

unix {
 LIBS += -lkexidb
}

win32 {

 LIBS += $$QKWLIB/kdecore$$KDELIB_SUFFIX $$QKWLIB/kdeui$$KDELIB_SUFFIX \
  $$QKWLIB/kdefx$$KDELIB_SUFFIX $$QKWLIB/kio$$KDELIB_SUFFIX $$QKWLIB/kstore$$KDELIB_SUFFIX \
  $$QKWLIB/kdewin32$$KDELIB_SUFFIX $$QKWLIB/kofficecore$$KDELIB_SUFFIX $$QKWLIB/kfile$$KDELIB_SUFFIX \
  $$QKWLIB/kparts$$KDELIB_SUFFIX \
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
