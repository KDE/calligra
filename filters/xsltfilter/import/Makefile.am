####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes) $(KDE_INCLUDES)/libxml2 $(KDE_INCLUDES)/libxslt
libxsltimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libxsltimport_la_LIBADD		= $(KOFFICE_LIBS) /opt/kde_devel/lib/libxslt.so /opt/kde_devel/lib/libxml2.so /opt/kde_devel/lib/libexslt.so /opt/kde_devel/lib/libxsltbreakpoint.so

####### Files

kde_module_LTLIBRARIES = libxsltimport.la

libxsltimport_la_SOURCES = xsltimport.cc xsltimportdia.cc xsltproc.cc xsltdialog.ui

noinst_HEADERS = xsltimport.h xsltimportdia.h xsltproc.h

######## Debug
check_PROGRAMS = xsltfilter
xsltfilter_SOURCES = xsltfilter.c
xsltfilter_LDADD = /opt/kde_devel/lib/libxslt.so /opt/kde_devel/lib/libxml2.so /opt/kde_devel/lib/libexslt.so /opt/kde_devel/lib/libxsltbreakpoint.so
xsltfilter_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = xslt_import.desktop
#xsl_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/xsltfilter.pot
