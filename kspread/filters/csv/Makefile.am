####### General stuff

# CSV filter uses QDom stuff from filters/lib

INCLUDES=	-I$(srcdir) -I$(top_srcdir)/filters/lib $(KOFFICE_INCLUDES) \
		$(all_includes) $(MICO_INCLUDES)
LDFLAGS	=	$(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(top_builddir)/filters/lib/libqtext.la $(LIB_KOFFICECORE) 

####### Files

bin_PROGRAMS = csvfilter

csvfilter_SOURCES = csvfilter.cc xmltree.cc main.cc

noinst_HEADERS = csvfilter.h xmltree.h main.h

METASOURCES = csvfilter.moc xmltree.moc main.moc

service_DATA = kspread_csv_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/text
