####### General stuff

INCLUDES= $(KSTORE_INCLUDES) -I$(srcdir)/../ $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(MICO_LDFLAGS) $(all_libraries)  -version-info 1:0:0
libkofficecore_la_LIBADD  = -lkparts ../store/libkstore.la -lkded $(LIB_KIO) $(LIB_KFILE)

####### Files

lib_LTLIBRARIES = libkofficecore.la

libkofficecore_la_SOURCES = koDocument.cc koView.cc koffice.idl \
	koFrame.cc koFilterManager.cc \
	koPrintExt.cc koMainWindow.cc koScanTools.cc \
	koScanPlugins.cc koApplication.cc koQueryTrader.cc \
	koUndo.cc qdom.cc qxml.cc qvariant.cc

include_HEADERS = koffice.h koView.h koFrame.h \
	koDocument.h koPrintExt.h koFactory.h \
	koMainWindow.h koScanTools.h koScanPlugins.h \
	koApplication.h koQueryTypes.h \
	koFilterManager.h qdom.h qxml.h koUnd.h qvariant.h

libkofficecore_la_METASOURCES = koFrame.moc koMainWindow.moc \
        koApplication.moc koUndo.moc

idl_DATA = koffice.idl

IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

koffice.idl: $(idldir)/openparts.idl $(idldir)/openparts_ui.idl $(idldir)/kom.idl $(top_srcdir)/lib/store/koStoreIf.idl

koDocument.cc: koffice.h
koView: koffice.h
