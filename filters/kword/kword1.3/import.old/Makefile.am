####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libkwordkword1dot3import.la


libkwordkword1dot3import_la_SOURCES = oowriterexport.cc ExportFilter.cc \
TagProcessing.cc KWEFStructures.cc\
ProcessDocument.cc KWEFKWordLeader.cc

libkwordkword1dot3import_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkwordkword1dot3import_la_LIBADD  = $(KOFFICE_LIBS)

noinst_HEADERS = TagProcessing.h KWEFStructures.h\
ProcessDocument.h KWEFKWordLeader.h ExportFilter.h

METASOURCES = AUTO

service_DATA = kword_kword1dot3_import.desktop

servicedir = $(kde_servicesdir)

