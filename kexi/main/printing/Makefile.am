include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkeximainprinting.la

libkeximainprinting_la_SOURCES = \
kexisimpleprintingengine.cpp \
kexisimpleprintingpart.cpp \
kexisimpleprintingwindowbase.ui \
kexisimpleprintingwindow.cpp \
kexisimpleprintpreviewwindow.cpp

noinst_HEADERS = kexisimpleprintpreviewwindow_p.h

libkeximainprinting_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkeximainprinting_la_LIBADD = \
	$(top_builddir)/kexi/3rdparty/kolibs/libkexikolibs.la \
	../../widget/libkexiextendedwidgets.la

libkeximainprinting_la_METASOURCES = AUTO

SUBDIRS = .

# set the include path for X, qt and KDE

INCLUDES= -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/main/printing \
	-I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/widget \
	-I$(top_builddir)/kexi/widget $(all_includes)

