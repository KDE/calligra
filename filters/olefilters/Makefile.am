
INCLUDES = -I$(srcdir) -I$(srcdir)/lib -I$(srcdir)/winword97 \
-I$(srcdir)/excel97 -I$(srcdir)/powerpoint97 $(KOFFICE_INCLUDES) \
$(KOFFICECORE_INCLUDES) $(all_includes)

libolefilter_la_LDFLAGS = -module -avoid-version -no-undefined
libolefilter_la_LIBADD = ./lib/libfilterbase.la ./lib/libklaola.la \
./winword97/libwinword97.la ./excel97/libexcel97.la \
./powerpoint97/libpowerpoint97.la $(LIB_KOFFICECORE)

####### Files

SUBDIRS = lib excel97 powerpoint97 winword97

lib_LTLIBRARIES = libolefilter.la

libolefilter_la_SOURCES = olefilter_factory.cc olefilter.cc

noinst_HEADERS = olefilter_factory.h olefilter.h

libolefilter_la_METASOURCES = AUTO

