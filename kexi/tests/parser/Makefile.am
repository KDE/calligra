include $(top_srcdir)/kexi/Makefile.global

bin_PROGRAMS = kexidbparser

noinst_PROGRAMS = kexidbparser

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

kexidbparser_SOURCES = main.cpp
kexidbparser_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la  $(all_libraries)
kexidbparser_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO) --no-undefined


