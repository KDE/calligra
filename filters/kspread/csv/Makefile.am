####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES)  $(all_includes)

####### Files

kde_module_LTLIBRARIES = libcsvimport.la libcsvexport.la

libcsvimport_la_SOURCES = csvimport.cc xmltree.cc csvdialog.cpp dialogui.ui
libcsvimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined 
libcsvimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la

libcsvexport_la_SOURCES = csvexport.cc csvexportdialog.cpp exportdialogui.ui
libcsvexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libcsvexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la

METASOURCES = AUTO

service_DATA = kspread_csv_import.desktop kspread_csv_export.desktop

servicedir = $(kde_servicesdir)
