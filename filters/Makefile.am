
INCLUDES = -I$(srcdir) -I$(srcdir)/lib -I$(srcdir)/winword97 \
-I$(srcdir)/excel97 $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
$(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ./lib/libfilterbase.la ./lib/libklaola.la \
./winword97/libwinword97.la ./excel97/libexcel97.la $(LIB_KOFFICECORE)

####### Files

SUBDIRS = lib excel97 powerpoint97 winword97

bin_PROGRAMS = ole_filter

ole_filter_SOURCES = main.cc olefilter.cc

noinst_HEADERS = main.h olefilter.h

ole_filter_METASOURCES = main.moc olefilter.moc
