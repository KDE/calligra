####### General stuff

INCLUDES= $(KOML_INCLUDES) $(KSTORE_INCLUDES) -I$(idldir) $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
LIBADD  = -lkom $(LIB_KFILE) $(LIBMICO) 

####### Files

lib_LTLIBRARIES = libkofficecore.la

libkofficecore_la_SOURCES = koDocument.cc koView.cc koffice.cc \
	koFrame.cc koIMR.cc koStream.cc koQueryTrader.cc \
	koMediator.cc koPrintExt.cc koMainWindow.cc koScanTools.cc \
	koScanPlugins.cc koApplication.cc koPOAMediator.cc trader.cc \
	koFilterManager.cc typerepo.cc

include_HEADERS = koffice.h koView.h koFrame.h koIMR.h \
	koStream.h koDocument.h koPrintExt.h koFactory.h \
	koMainWindow.h koMediator.h koScanTools.h koScanPlugins.h \
	koApplication.h koPOAMediator.h trader.h koQueryTrader.h \
	koQueryTypes.h koFilterManager.h typerepo.h

libkofficecore_la_METASOURCES = koFrame.moc koMainWindow.moc koApplication.moc

DISTCLEANFILES = $(libkofficecore_la_METASOURCES)

idl_DATA = koffice.idl
idl_HEADERS =  koffice.h

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

$(srcdir)/koFrame.cc: koffice.h
$(srcdir)/koMainWindow.cc: koffice.h
$(srcdir)/koApplication.cc: koffice.h
$(srcdir)/koDocument.cc: koDocument.h
$(srcdir)/koIMR.cc: trader.h
$(srcdir)/koQueryTrader.cc: typerepo.h trader.h
$(srcdir)/koFilterManager.cc: koffice.h
koffice.cc: koffice.h
koffice.h: koffice.idl $(idldir)/openparts.idl $(idldir)/openparts_ui.idl $(idldir)/kom.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/koffice.idl
trader.cc: trader.h
trader.h: trader.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/trader.idl
typerepo.cc: typerepo.h
typerepo.h: typerepo.idl trader.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/typerepo.idl

