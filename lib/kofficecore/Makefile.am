####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= $(KSTORE_INCLUDES) $(all_includes)
libkofficecore_la_LIBADD  = \
	$(top_builddir)/lib/store/libkstore.la $(LIB_KPARTS) $(LIB_KDEPRINT)

####### Files

lib_LTLIBRARIES = libkofficecore.la libkodocinfopropspage.la

libkofficecore_la_SOURCES = koDocument.cc \
	koFilterManager.cc \
	koMainWindow.cc koPrintPreview.cc \
	koApplication.cc koQueryTrader.cc \
	koDataTool.cc koFilter.cc \
	koDocumentInfo.cc koFilterDialog.cc \
	koView.cc koFrame.cc handler.cpp koDocumentChild.cc \
	koDocumentInfoDlg.cc koFactory.cc koChild.cc \
	KoApplicationIface.cc KoApplicationIface.skel \
	KoDocumentIface.cc KoDocumentIface.skel KoViewIface.cc KoViewIface.skel

libkofficecore_la_LDFLAGS = $(KDE_LDFLAGS)  -version-info 1:0:0 -no-undefined

include_HEADERS = koFilter.h \
	koDocument.h \
	koMainWindow.h koPrintPreview.h \
	koApplication.h koQueryTrader.h \
	koFilterManager.h \
	koDataTool.h koDocumentInfo.h \
	koFilterDialog.h koView.h koFrame.h handler.h koDocumentChild.h \
	koDocumentInfoDlg.h koFactory.h koChild.h \
	KoApplicationIface.h KoDocumentIface.h KoViewIface.h \
	koImage.h koImageCollection.h koprinter.h

noinst_HEADERS = koDocument_p.h

#libkofficecore_la_METASOURCES = AUTO

libkodocinfopropspage_la_SOURCES = koDocInfoPropsFactory.cc
libkodocinfopropspage_la_LIBADD = libkofficecore.la
libkodocinfopropspage_la_LDFLAGS = $(all_libraries) -module -no-undefined
#libkodocinfopropspage_la_METASOURCES = AUTO

METASOURCES = AUTO

rcdir = $(kde_datadir)/koffice
rc_DATA = koffice_shell.rc

service_DATA = kodocinfopropspage.desktop
servicedir = $(kde_servicesdir)
