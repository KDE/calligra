####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread -I$(srcdir)/../libkspreadexport $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) $(KOSCRIPT_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libkspreadexcelexport.la

libkspreadexcelexport_la_SOURCES = excelexport.cc
libkspreadexcelexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkspreadexcelexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la ../libkspreadexport/libkspreadexport.la

METASOURCES = AUTO

service_DATA = kspread_excel_export.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kspreadexcelfilter.pot
