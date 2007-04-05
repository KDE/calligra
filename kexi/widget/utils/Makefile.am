include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexiguiutils.la
libkexiguiutils_la_SOURCES = kexisharedactionclient.cpp kexirecordnavigator.cpp \
	kexigradientwidget.cpp kexirecordmarker.cpp kexidisplayutils.cpp \
	kexiflowlayout.cpp kexidatetimeformatter.cpp kexitooltip.cpp kexiarrowtip.cpp \
	kexidropdownbutton.cpp kexicomboboxdropdownbutton.cpp kexicontextmenuutils.cpp

libkexiguiutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
libkexiguiutils_la_LIBADD = $(LIB_KDEUI)

SUBDIRS = .

# set the include path for X, qt and KDE - all_includes must remain last!
INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/widget/utils $(all_includes)

METASOURCES = AUTO

noinst_HEADERS = kexigradientwidget.h
