####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
-I$(top_srcdir)/lib/kscript -I$(top_srcdir)/lib/koscript $(all_includes)

####### Files

kde_module_LTLIBRARIES = libkspreadhtmlexport.la

libkspreadhtmlexport_la_SOURCES = htmlexport.cc htmlexport_factory.cc
libkspreadhtmlexport_la_LDFLAGS = -module -avoid-version -no-undefined
libkspreadhtmlexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadpart.la

METASOURCES = AUTO

service_DATA = kspread_html_export.desktop

servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kspreadhtmlfilter.pot
