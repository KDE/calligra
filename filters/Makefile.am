
INCLUDES = -I$(srcdir) -I$(srcdir)/winword97 $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
           $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ./winword97/libwinword97.la $(LIB_KOFFICECORE)

####### Files

SUBDIRS = excel97 powerpoint97 winword97

bin_PROGRAMS = ole_filter

ole_filter_SOURCES = main.cc olefilter.cc klaola.cc ktar.cc

noinst_HEADERS = main.h olefilter.h klaola.h ktar.h

METASOURCES = main.moc olefilter.moc

####### Build rules

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h
