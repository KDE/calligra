####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../sidewinder $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libexcelimport.la

libexcelimport_la_SOURCES = excelimport.cc 
libexcelimport_la_LIBADD = $(KOFFICE_LIBS) ../sidewinder/libsidewinder.la
libexcelimport_la_LDFLAGS = -module $(KDE_PLUGIN)
noinst_HEADERS = excelimport.h

METASOURCES = AUTO

service_DATA = kspread_excel_import.desktop
servicedir = $(kde_servicesdir)
