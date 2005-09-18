include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/scriptingcore $(all_includes)

kde_module_LTLIBRARIES = krosskexiapp.la

krosskexiapp_la_SOURCES = \
    kexiappmainwindow.cpp \
    kexiappmodule.cpp

krosskexiapp_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
krosskexiapp_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(top_builddir)/kexi/scriptingcore/api/libkrossapi.la \
    $(top_builddir)/kexi/scriptingcore/main/libkrossmain.la \
    $(top_builddir)/kexi/core/libkexicore.la

METASOURCES = AUTO
SUBDIRS = .
