
INCLUDES = -I$(srcdir) -I$(srcdir)/lib \
-I$(srcdir)/powerpoint97 $(KOFFICE_INCLUDES) \
$(all_includes)

libolefilter_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libolefilter_la_LIBADD = ./lib/libfilterbase.la ./lib/libklaola.la \
./powerpoint97/libpowerpoint97.la \
$(LIB_KOFFICECORE)

####### Files

SUBDIRS = lib powerpoint97

kde_module_LTLIBRARIES = libolefilter.la

libolefilter_la_SOURCES = olefilter.cc

noinst_HEADERS = olefilter.h

libolefilter_la_METASOURCES = AUTO

