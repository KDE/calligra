####### General stuff

SUBDIRS = . tests
KDE_CXXFLAGS = $(USE_RTTI) $(WOVERLOADED_VIRTUAL)
INCLUDES= $(KSTORE_INCLUDES) $(KWMF_INCLUDES) $(all_includes)
libkofficecore_la_LIBADD  = $(LIB_KSTORE) $(LIB_KOWMF) $(LIB_KPARTS) \
	$(LIB_KDEPRINT) $(LIB_KABC) $(LIB_KWMF)

####### Files

lib_LTLIBRARIES = libkofficecore.la 
kde_module_LTLIBRARIES = kodocinfopropspage.la

libkofficecore_la_SOURCES = koDocument.cc \
	koGlobal.cc koUnit.cc koFilterManager.cc \
	koMainWindow.cc \
	koApplication.cc koQueryTrader.cc \
	koFilter.cc koFilterChain.cpp \
	koPictureKey.cc koPictureBase.cc koPicture.cc koPictureShared.cc \
	koPictureImage.cc koPictureClipart.cc koPictureCollection.cc \
	koPictureEps.cc koPictureWmf.cc \
	koDocumentInfo.cc \
	koView.cc koFrame.cc handler.cpp koDocumentChild.cc \
	koDocumentInfoDlg.cc koFactory.cc koChild.cc \
	KoApplicationIface.cc KoApplicationIface.skel \
	KoDocumentIface.cc KoDocumentIface.skel KoViewIface.cc KoViewIface.skel \
	KoMainWindowIface.cc KoMainWindowIface.skel kofficeversion.cc koOasisStyles.cc \
	koStyleStack.cc koGenStyles.cpp koOasisSettings.cc koPageLayout.cpp \
	kofiledialog.cpp koxmlns.cpp kodom.cpp koversiondialog.cc

libkofficecore_la_LDFLAGS = $(all_libraries) -version-info 2:0:0 -no-undefined

include_HEADERS = handler.h \
	koFilter.h koFilterChain.h \
	koGlobal.h koUnit.h koDocument.h \
	koMainWindow.h \
	koApplication.h koQueryTrader.h \
	koFilterManager.h \
	koDocumentInfo.h \
	koView.h koFrame.h koDocumentChild.h \
	koDocumentInfoDlg.h koFactory.h koChild.h \
	KoApplicationIface.h KoDocumentIface.h KoViewIface.h KoMainWindowIface.h \
	koPictureKey.h koPicture.h koPictureCollection.h kofficeversion.h koOasisStyles.h \
	koStyleStack.h koGenStyles.h koOasisSettings.h koPageLayout.h koxmlns.h kodom.h koversiondialog.h

noinst_HEADERS = koDocument_p.h koFilterManager_p.h \
	koPictureShared.h koPictureBase.h koPictureImage.h koPictureClipart.h \
	koPictureEps.h koPictureWmf.h

include_HEADERS = kofficemacros.h

kodocinfopropspage_la_SOURCES = koDocInfoPropsFactory.cc
kodocinfopropspage_la_LIBADD = ./libkofficecore.la
kodocinfopropspage_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

METASOURCES = AUTO

rcdir = $(kde_datadir)/koffice
rc_DATA = koffice_shell.rc

service_DATA = kodocinfopropspage.desktop
servicedir = $(kde_servicesdir)
