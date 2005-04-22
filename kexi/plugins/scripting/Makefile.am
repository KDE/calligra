include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_script.la

kexihandler_script_la_SOURCES = kexiscriptpart.cpp \
	kexiscriptview.cpp kexiscripteditor.cpp
kexihandler_script_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kexihandler_script_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la

INCLUDES = -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget $(all_includes)

METASOURCES = AUTO

SUBDIRS = .

include ../Makefile.common

noinst_HEADERS = kexiscriptpart.h kexiscriptview.h kexiscripteditor.h
