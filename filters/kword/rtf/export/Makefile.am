####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
librtfexport_la_LDFLAGS	= -module $(KDE_PLUGIN)
librtfexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = librtfexport.la

librtfexport_la_SOURCES = rtfexport.cc rtfexport_factory.cc kwExport.cc docinfoExport.cc tagprocessing.cc kiExport.cc kiRTFExport.cc

noinst_HEADERS = rtfexport.h rtfexport_factory.h kwExport.h docinfoExport.h tagprocessing.h kiExport.h kiRTFExport.h

METASOURCES = AUTO

service_DATA = kword_rtf_export.desktop ki_rtf_export.desktop
servicedir = $(kde_servicesdir)

