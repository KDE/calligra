include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexiutils.la
libkexiutils_la_SOURCES = identifier.cpp

libkexiutilsincludedir=$(includedir)/kexidb
libkexiutilsinclude_HEADERS = tristate.h identifier.h

libkexiutils_la_LDFLAGS = \
  $(KDE_RPATH) $(all_libraries) \
  $(VER_INFO) -Wnounresolved -no-undefined

libkexiutils_la_LIBADD = $(LIB_QT)

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

METASOURCES = AUTO
