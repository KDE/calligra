####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= $(KOML_INCLUDES) $(KSTORE_INCLUDES) $(all_includes)
libkofficecore_la_LIBADD  = \
	$(top_builddir)/lib/koml/libkoml.la \
	$(top_builddir)/lib/store/libkstore.la -lkparts $(LIB_KIMGIO) $(LIB_KFILE)

####### Files

lib_LTLIBRARIES = libkofficecore.la

libkofficecore_la_SOURCES = koDocument.cc \
	koStream.cc koFilterManager.cc \
	koMainWindow.cc \
	koApplication.cc koQueryTrader.cc \
	koUndo.cc koIcons.cc \
	koDataTool.cc koFilter.cc \
	koDocumentInfo.cc koFilterDialog.cc \
	koView.cc koFrame.cc handler.cpp koDocumentChild.cc 
libkofficecore_la_LDFLAGS = $(all_libraries)  -version-info 1:0:0 -no-undefined

include_HEADERS = koFilter.h \
	koStream.h koDocument.h \
	koMainWindow.h \
	koApplication.h koQueryTypes.h \
	koFilterManager.h koUndo.h koIcons.h \
	koDataTool.h koDocumentInfo.h \
	koFilterDialog.h koView.h koFrame.h handler.h koDocumentChild.h

libkofficecore_la_METASOURCES = AUTO

rcdir = $(kde_datadir)/koffice
rc_DATA = koffice_shell.rc
