####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

libkwordexportfiltersold_la_LDFLAGS = $(all_libraries) -version-info 0:0:0 -no-undefined

libkwordexportfiltersold_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libkwordexportfiltersold.la

libkwordexportfiltersold_la_SOURCES = TagProcessing.cc KWEFStructures.cc\
ProcessDocument.cc KWEFUtil.cc KWEFKWordLeader.cc KWEFBaseWorker.cc

noinst_HEADERS = TagProcessing.h KWEFStructures.h\
ProcessDocument.h KWEFUtil.h KWEFKWordLeader.h KWEFBaseWorker.h

METASOURCES = AUTO
