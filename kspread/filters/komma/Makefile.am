
INCLUDES = $(PYTHONINC) $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) 
LDFLAGS	= $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICEUI) $(LIBPYTHON)

####### Files

bin_PROGRAMS = kspread_filter

kspread_filter_SOURCES = main.cc

noinst_HEADERS = main.h

METASOURCES = main.moc

service_DATA = kspread_filter_import.desktop
servicedir = $(kde_servicesdir)

datasdir = $(kde_datadir)/koffice/filter
datas_DATA = komma.py

####### Implicit rules

####### Build rules

messages: 
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h

