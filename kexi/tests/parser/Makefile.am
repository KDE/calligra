bin_PROGRAMS = kexidbparser

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

kexidbparser_SOURCES = main.cpp
kexidbparser_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la $(top_srcdir)/kexi/kexidb/parser/libkexidbparser.la  $(all_libraries)
kexidbparser_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

include $(top_srcdir)/kexi/Makefile.global

