####### General stuff

INCLUDES= -I$(top_srcdir)/lib/koml -I$(top_srcdir)/lib/store \
	-I$(idldir) $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
LIBADD  = -lkom $(LIB_KFILE) $(LIBMICO) 

####### Files

lib_LTLIBRARIES = libkofficecore.la

libkofficecore_METASOURCES = USE_AUTOMOC cc

libkofficecore_la_SOURCES = koDocument.cc koView.cc koffice.cc \
	koFrame.cc koIMR.cc koStream.cc koQueryTrader.cc \
	koMediator.cc koPrintExt.cc koMainWindow.cc koScanTools.cc \
	koScanPlugins.cc koApplication.cc koPOAMediator.cc trader.cc \
	koFilterManager.cc 

include_HEADERS = koffice.h koView.h koFrame.h koIMR.h \
	koStream.h koDocument.h koPrintExt.h koFactory.h \
	koMainWindow.h koMediator.h koScanTools.h koScanPlugins.h \
	koApplication.h koPOAMediator.h trader.h koQueryTrader.h \
	koQueryTypes.h koFilterManager.h

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/store -I$(idldir)

idl_DATA = koffice.idl
idl_HEADERS = koffice.h

koIMR.cc: trader.h
koDocument.cc: koDocument.h
moc_koMainWindow.cc: koffice.h
koDocument.h: koffice.h
koffice.cc: koffice.h
koffice.h: koffice.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/koffice.idl
trader.cc: trader.h
trader.h: $(top_srcdir)/lib/interfaces/trader.idl
	$(IDL) $(IDL_FLAGS) $(top_srcdir)/lib/interfaces/trader.idl

