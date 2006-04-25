noinst_LTLIBRARIES = libkeximacroactions.la

libkeximacroactions_la_SOURCES = \
    openobject.cpp

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkeximacroactions_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkeximacroactions_la_LIBADD = \
    $(top_builddir)/kexi/plugins/macros/lib/libkomacro.la \
    $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

libkeximacroactions_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = $(all_includes)
