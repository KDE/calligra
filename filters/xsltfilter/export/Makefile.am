####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes) $(KDE_INCLUDES)/libxml2 $(KDE_INCLUDES)/libxslt
libxsltexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libxsltexport_la_LIBADD		= $(KOFFICE_LIBS) /opt/kde_devel/lib/libxslt.so /opt/kde_devel/lib/libxml2.so /opt/kde_devel/lib/libexslt.so /opt/kde_devel/lib/libxsltbreakpoint.so

####### Files

kde_module_LTLIBRARIES = libxsltexport.la

libxsltexport_la_SOURCES = xsltexport.cc xsltexportdia.cc xsltproc.cc xsltdialog.ui

noinst_HEADERS = xsltexport.h xsltexportdia.h xsltproc.h

######## Debug
check_PROGRAMS = xsltfilter
xsltfilter_SOURCES = xsltfilter.c
xsltfilter_LDADD = /opt/kde_devel/lib/libxslt.so /opt/kde_devel/lib/libxml2.so /opt/kde_devel/lib/libexslt.so /opt/kde_devel/lib/libxsltbreakpoint.so
xsltfilter_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = xslt_export.desktop

servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/xsltfilter.pot
