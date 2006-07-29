####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

libkwordexportfilters_la_LDFLAGS = $(all_libraries) -version-info 1:1:0 -no-undefined

libkwordexportfilters_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libkwordexportfilters.la

libkwordexportfilters_la_SOURCES = TagProcessing.cc KWEFStructures.cc\
ProcessDocument.cc KWEFUtil.cc KWEFKWordLeader.cc KWEFBaseWorker.cc

noinst_HEADERS = TagProcessing.h KWEFStructures.h\
ProcessDocument.h KWEFUtil.h KWEFKWordLeader.h KWEFBaseWorker.h

METASOURCES = AUTO
