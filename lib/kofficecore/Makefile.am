####### General stuff

INCLUDES= $(KOML_INCLUDES) $(KSTORE_INCLUDES) -I$(srcdir)/../ -I$(idldir) $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(MICO_LDFLAGS) $(all_libraries)  -version-info 1:0:0
libkofficecore_la_LIBADD  = -lkparts ../koml/libkoml.la ../store/libkstore.la -lkded $(LIB_KIO) $(LIB_KFILE)

####### Files

lib_LTLIBRARIES = libkofficecore.la

libkofficecore_la_SOURCES = koDocument.cc koView.cc koffice.cc \
	koFrame.cc koIMR.cc koStream.cc koFilterManager.cc \
	koPrintExt.cc koMainWindow.cc koScanTools.cc \
	koScanPlugins.cc koApplication.cc trader.cc \
	typerepo.cc koQueryTrader.cc

include_HEADERS = koffice.h koView.h koFrame.h koIMR.h \
	koStream.h koDocument.h koPrintExt.h koFactory.h \
	koMainWindow.h koScanTools.h koScanPlugins.h \
	koApplication.h trader.h koQueryTrader.h \
	koQueryTypes.h koFilterManager.h typerepo.h

libkofficecore_la_METASOURCES = koFrame.moc koMainWindow.moc koApplication.moc

DISTCLEANFILES = typerepo.cc typerepo.h trader.h trader.cc koffice.cc koffice.h

idl_DATA = koffice.idl
idl_HEADERS =  koffice.h

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

#koffice.cc: koffice.h
#koffice.h: koffice.idl $(idldir)/openparts.idl $(idldir)/openparts_ui.idl $(idldir)/kom.idl $(micodir)/include/CORBA.h
#	$(IDL) $(IDL_FLAGS) $(srcdir)/koffice.idl
trader.cc: trader.h
trader.h: trader.idl $(micodir)/include/CORBA.h
	$(IDL) $(IDL_FLAGS) $(srcdir)/trader.idl
typerepo.cc: typerepo.h
typerepo.h: typerepo.idl trader.idl $(micodir)/include/CORBA.h
	$(IDL) $(IDL_FLAGS) $(srcdir)/typerepo.idl

