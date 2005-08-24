include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_script.la

if compile_kross_plugin
KROSS_DEFINES=-DKEXI_KROSS_SUPPORT
KROSS_INCLUDE=-I$(top_srcdir)/kexi/scripting
KROSS_LIB=$(top_builddir)/kexi/scripting/main/libkrossmain.la
endif

AM_CPPFLAGS=$(KROSS_DEFINES)

kexihandler_script_la_SOURCES = \
    kexiscriptpart.cpp kexiscripteditor.cpp kexiscriptdesignview.cpp kexiscripttextview.cpp

kexihandler_script_la_LDFLAGS = \
    $(KDE_PLUGIN) -module -no-undefined -Wnounresolved $(all_libraries) $(VER_INFO)

kexihandler_script_la_LIBADD = \
    $(top_builddir)/kexi/core/libkexicore.la \
    $(top_builddir)/kexi/main/libkeximain.la \
    $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
    $(top_builddir)/lib/koproperty/libkoproperty.la \
    $(KROSS_LIB)

INCLUDES = \
    $(KOFFICE_INCLUDES) \
    -I$(top_srcdir)/lib  \
    -I$(top_srcdir)/kexi/core \
    -I$(top_srcdir)/kexi/main \
    -I$(top_srcdir)/kexi \
    -I$(top_srcdir)/kexi/widget \
    $(KROSS_INCLUDE) \
    $(all_includes)

METASOURCES = AUTO

SUBDIRS = .

include ../Makefile.common

noinst_HEADERS = kexiscriptpart.h kexiscripteditor.h kexiscriptdesignview.h kexiscripttextview.h
