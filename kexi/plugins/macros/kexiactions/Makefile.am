include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkeximacroactions.la

libkeximacroactions_la_SOURCES = \
    openaction.cpp \
    executeaction.cpp \
    navigateaction.cpp \
    messageaction.cpp

libkeximacroactions_la_CXXFLAGS = $(USE_EXCEPTIONS)

libkeximacroactions_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkeximacroactions_la_LIBADD = \
    $(top_builddir)/kexi/plugins/macros/lib/libkomacro.la \
    $(top_builddir)/kexi/core/libkexicore.la \
    $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

libkeximacroactions_la_METASOURCES = AUTO
SUBDIRS = .

INCLUDES = \
    -I$(top_srcdir)/kexi/core \
    -I$(top_srcdir)/kexi \
    $(all_includes)
