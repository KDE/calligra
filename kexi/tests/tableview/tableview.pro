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

# test specific:
 LIBS += \
	$$KDELIBDESTDIR/kexidb$$KEXILIB_SUFFIX \
	$$KDELIBDESTDIR/kexiwidgets$$KEXILIB_SUFFIX

#allow to select target independently from debug information
 CONFIG += console
 CONFIG -= windows
}

DESTDIR = .
TARGET		= tableview

SOURCES = \
main.cpp

HEADERS		= 
