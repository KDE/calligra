
INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) 
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICECORE)

####### Files

bin_PROGRAMS = kword_ascii

kword_ascii_SOURCES = main.cc

noinst_HEADERS = main.h

METASOURCES = main.moc

service_DATA = kword_ascii_import.desktop
servicedir = $(kde_servicesdir)

datasdir = $(kde_datadir)/koffice/filter

#bin_SCRIPTS = kword_ascii

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h

