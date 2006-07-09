include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_script.la

kexihandler_script_la_SOURCES = \
    kexiscriptpart.cpp kexiscripteditor.cpp kexiscriptdesignview.cpp

kexihandler_script_la_LDFLAGS = \
    $(KDE_PLUGIN) -module -no-undefined -Wnounresolved $(all_libraries) $(VER_INFO)

kexihandler_script_la_LIBADD = \
    $(top_builddir)/lib/kross/main/libkrossmain.la \
    $(top_builddir)/kexi/core/libkexicore.la \
    $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
    $(top_builddir)/lib/koproperty/libkoproperty.la

INCLUDES = \
    $(KOFFICE_INCLUDES) \
    -I$(top_srcdir)/lib  \
    -I$(top_srcdir)/kexi/core \
    -I$(top_srcdir)/kexi \
    -I$(top_srcdir)/kexi/widget \
    $(all_includes)

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexiscripthandler.desktop

rcdir = $(kde_datadir)/kexi
rc_DATA = kexiscriptpartui.rc kexiscriptpartinstui.rc

METASOURCES = AUTO

SUBDIRS = .

include ../../Makefile.common

noinst_HEADERS = kexiscriptpart.h kexiscripteditor.h kexiscriptdesignview.h
