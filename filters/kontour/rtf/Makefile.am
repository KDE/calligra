####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libkontourrtfexport_la_LDFLAGS	= -module $(KDE_PLUGIN)
libkontourrtfexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libkontourrtfexport.la

libkontourrtfexport_la_SOURCES = rtfexport.cc kwExport.cc docinfoExport.cc tagprocessing.cc kiExport.cc kiRTFExport.cc

noinst_HEADERS = rtfexport.h kwExport.h docinfoExport.h tagprocessing.h kiExport.h kiRTFExport.h

METASOURCES = AUTO

service_DATA = kontour_rtf_export.desktop
servicedir = $(kde_servicesdir)

