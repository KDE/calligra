
INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) $(all_includes) 
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KOFFICECORE)

####### Files

bin_PROGRAMS = kword_html

kword_html_SOURCES = main.cc kword_xml2html.cc xmlparse.cc

noinst_HEADERS = main.h kword_xml2html.h 

METASOURCES = main.moc

service_DATA = kword_html_export.desktop kword_html_import.desktop
servicedir = $(kde_servicesdir)

datasdir = $(kde_datadir)/koffice/filter

#bin_SCRIPTS = kword_ascii

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h

