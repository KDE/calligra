####### General stuff

INCLUDES=	-I$(srcdir) -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
		$(all_includes) $(MICO_INCLUDES)
LDFLAGS	=	$(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(top_builddir)/filters/lib/libfilterbase.la $(top_builddir)/filters/lib/libqtext.la \
	$(LIB_KOFFICECORE) 

####### Files

bin_PROGRAMS = csvfilter

csvfilter_SOURCES = csvfilter.cc xmltree.cc main.cc

noinst_HEADERS = csvfilter.h xmltree.h main.h

METASOURCES = csvfilter.moc xmltree.moc main.moc

service_DATA = csv_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/application
