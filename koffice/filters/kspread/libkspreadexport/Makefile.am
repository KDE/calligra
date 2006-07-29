####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread -I$(srcdir)/../libkspreadexport $(KOFFICE_INCLUDES) \
	          $(KOTEXT_INCLUDES) $(all_includes)

libkspreadexport_la_LDFLAGS = $(all_libraries) -module -version-info 1:0:0 -no-undefined

libkspreadexport_la_LIBADD = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la

####### Files

lib_LTLIBRARIES = libkspreadexport.la

libkspreadexport_la_SOURCES = KSpreadLeader.cc KSpreadBaseWorker.cc

noinst_HEADERS = KSpreadLeader.h KSpreadBaseWorker.h

METASOURCES = AUTO
