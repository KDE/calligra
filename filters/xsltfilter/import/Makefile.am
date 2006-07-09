####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES)  $(all_includes) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS)

####### Files

kde_module_LTLIBRARIES = libxsltimport.la

libxsltimport_la_SOURCES = xsltimport.cc xsltimportdia.cc xsltproc.cc xsltdialog.ui
libxsltimport_la_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries) -module $(KDE_PLUGIN)
libxsltimport_la_LIBADD	= $(KOFFICE_LIBS) $(LIBXSLT_LIBS) $(LIBXML_LIBS)

noinst_HEADERS = xsltimport.h xsltimportdia.h xsltproc.h

######## Debug
#check_PROGRAMS = xsltfilter
#xsltfilter_SOURCES = xsltfilter.c
#xsltfilter_LDADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)
#xsltfilter_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries)

METASOURCES = AUTO

service_DATA = xslt_import.desktop
servicedir = $(kde_servicesdir)
