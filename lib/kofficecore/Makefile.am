####### General stuff

INCLUDES= -I../koml -I../store -I$(idldir) $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
LIBADD  = -lkom -lkfile  $(LIBMICO) $(LIB_KDEUI)

####### Files

lib_LTLIBRARIES = libkofficecore.la
BUILT_SOURCES =  moc_koFrame.cc moc_koMainWindow.cc
libkofficecore_la_SOURCES = koDocument.cc koView.cc koffice.cc koFrame.cc koIMR.cc koScanParts.cc koStream.cc \
			    koMediator.cc koPrintExt.cc koMainWindow.cc koScanTools.cc koScanPlugins.cc \
			    $(BUILT_SOURCES)
include_HEADERS = koffice.h koView.h koFrame.h koIMR.h koScanParts.h koStream.h koDocument.h koPrintExt.h koFactory.h \
		  koMainWindow.h koMediator.h koScanTools.h koScanPlugins.h

idl_DATA = koffice.idl
idl_HEADERS =  koffice.h

SUFFICES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

moc_koFrame.cc: koFrame.h
	$(MOC) koFrame.h -o moc_koFrame.cc

moc_koMainWindow.cc: koMainWindow.h
	$(MOC) koMainWindow.h -o moc_koMainWindow.cc

koDocument.cc: koDocument.h
koDocument.h: koffice.h
koffice.cc: koffice.h
koffice.h: koffice.idl
	$(IDL) --query-server-for-narrow --relative-paths -I../store -I$(idldir) koffice.idl

