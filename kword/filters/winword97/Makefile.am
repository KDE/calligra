
INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICECORE)

####### Files

bin_PROGRAMS = kword_winword97

kword_winword97_SOURCES = main.cc

noinst_HEADERS = main.h

METASOURCES = main.moc

service_DATA = kword_winword97_import.desktop
servicedir = $(kde_servicesdir)

datasdir = $(kde_datadir)/koffice/filter

data_DATA = winword97.kdelnk
datadir = $(kde_mimedir)/application

bin_SCRIPTS = kword_winword97

####### Build rules

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h
