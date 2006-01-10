include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi $(KROSS_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = krosskexiapp.la

krosskexiapp_la_SOURCES = \
    kexiapppart.cpp \
    kexiappmainwindow.cpp \
    kexiappmodule.cpp

krosskexiapp_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
krosskexiapp_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(LIB_KROSS_API) \
    $(LIB_KROSS_MAIN) \
    $(top_builddir)/kexi/core/libkexicore.la

METASOURCES = AUTO
SUBDIRS = .
