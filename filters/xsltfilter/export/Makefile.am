####### General stuff
SUBDIRS = xsl

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS)

####### Files

kde_module_LTLIBRARIES = libxsltexport.la

libxsltexport_la_SOURCES = xsltexport.cc xsltexportdia.cc xsltproc.cc xsltdialog.ui
libxsltexport_la_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries) -module $(KDE_PLUGIN)
libxsltexport_la_LIBADD	= $(KOFFICE_LIBS) $(LIBXSLT_LIBS) $(LIBXML_LIBS)

noinst_HEADERS = xsltexport.h xsltexportdia.h xsltproc.h

######## Debug
#check_PROGRAMS = xsltfilter
#xsltfilter_SOURCES = xsltfilter.cc
#xsltfilter_LDADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)
#xsltfilter_LDFLAGS = $(KDE_RPATH) $(LIBXML_RPATH) $(LIBXSLT_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = xslt_export.desktop

servicedir = $(kde_servicesdir)
