TEMPLATE	= app

include( $(QKW)/kwcommon.pro )

CONFIG		+= qt warn_on release
DEPENDPATH	= ../../include

system( bash kmoc )

TARGET = finddialogtest
DESTDIR=.

system( bash kmoc )
system( bash kdcopidl )

SOURCES = kexifinddialog.cpp main.cpp

FORMS = kexifinddialogbase.ui
