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

# test specific:
 LIBS += \
	$$QKWLIB/kexidb$$KEXILIB_SUFFIX \
	$$QKWLIB/kexiwidgets$$KEXILIB_SUFFIX

#allow to select target independently from debug information
 CONFIG += console
 CONFIG -= windows
}

DESTDIR = .
TARGET		= tableview

SOURCES = \
main.cpp

HEADERS		= 
