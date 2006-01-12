####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libkspreadhtmlexport.la

libkspreadhtmlexport_la_SOURCES = htmlexport.cc exportdialog.cc exportwidget.ui
libkspreadhtmlexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkspreadhtmlexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la

METASOURCES = AUTO

service_DATA = kspread_html_export.desktop

servicedir = $(kde_servicesdir)
