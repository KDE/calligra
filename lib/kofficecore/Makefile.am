####### General stuff

INCLUDES= -I../koml -I../store -I$(idldir) $(all_includes) $(MICO_INCLUDES)
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

SRCMETA = koFrame.moc koMainWindow.moc koApplication.moc

idl_DATA = koffice.idl
idl_HEADERS =  koffice.h

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I./ -I../store -I../interfaces -I$(idldir)

koFrame.moc: koFrame.h
koFrame.cc: koFrame.moc koffice.h
koMainWindow.moc: koMainWindow.h
koMainWindow.cc: koMainWindow.moc koffice.h
koApplication.moc: koApplication.h
koApplication.cc: koApplication.moc koffice.h
koDocument.cc: koDocument.h
koDocument.h: koffice.h
koQueryTrader.h: typerepo.h
koffice.cc: koffice.h
koffice.h: koffice.idl $(idldir)/openparts.idl $(idldir)/openparts_ui.idl $(idldir)/kom.idl
	$(IDL) $(IDL_FLAGS) koffice.idl
trader.cc: trader.h
trader.h: trader.idl
	$(IDL) $(IDL_FLAGS) trader.idl
typerepo.cc: typerepo.h
typerepo.h: typerepo.idl trader.idl
	$(IDL) $(IDL_FLAGS) typerepo.idl

