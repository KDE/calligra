KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(kde_includes)/arts $(all_includes)

bin_SCRIPTS = kprconverter.pl

## The part
kde_module_LTLIBRARIES = libkpresenterpart.la
libkpresenterpart_la_SOURCES = searchdia.cc notebar.cc backdia.cc webpresentation.cc kpresenter_view.cc \
	kpresenter_doc.cc \
	page.cc  styledia.cc kpresenter_sound_player.cc \
	pgconfdia.cc effectdia.cc kprcommand.cc \
	kpresenter_utils.cc rotatedia.cc \
	shadowdia.cc presstructview.cc \
	confpiedia.cc confrectdia.cc \
	kpobject.cc kplineobject.cc kprectobject.cc kpellipseobject.cc \
	kpautoformobject.cc kpclipartobject.cc kptextobject.cc \
	kppixmapobject.cc kppieobject.cc kppartobject.cc kpgroupobject.cc \
	kpbackground.cc kpclipartcollection.cc kpgradient.cc \
	kpgradientcollection.cc \
	gotopage.cc footer_header.cc \
	kpresenter_factory.cc \
	KPresenterDocIface.skel KPresenterDocIface.cc \
	KPresenterViewIface.skel KPresenterViewIface.cc \
	KPresenterObjectIface.skel KPresenterObjectIface.cc \
	KPresenterBackgroundIface.skel KPresenterBackgroundIface.cc \
	textdialog.ui sidebar.cc \
	insertpagedia.ui generalconfig.ui kpimage.cc \
	kpresenter_dlg_config.cc kprvariable.cc kprdrag.cc \
	kprtextdocument.cc kpfreehandobject.cc kppolylineobject.cc \
	kpquadricbeziercurveobject.cc kpcubicbeziercurveobject.cc \
	confpolygondia.cc kppolygonobject.cc

libkpresenterpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkpresenterpart_la_LIBADD = autoformEdit/libaf.la $(LIB_KOFFICEUI) $(LIB_KOTEXT) -lkspell  -lqtmcop -lkmedia2_idl -lsoundserver_idl -lartskde
libkpresenterpart_la_METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = kpresenter.la
kpresenter_la_SOURCES = main.cc
kpresenter_la_LDFLAGS = $(all_libraries) -avoid-version -module
kpresenter_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kpresenter
kpresenter_SOURCES = dummy.cc
kpresenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kpresenter_LDADD = kpresenter.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = kpresenter_doc.h kpresenter_view.h \
		backdia.h page.h styledia.h \
		global.h kprcommand.h kpresenter_sound_player.h \
		pgconfdia.h effectdia.h kpresenter_utils.h \
		rotatedia.h shadowdia.h presstructview.h \
		confpiedia.h confrectdia.h  searchdia.h \
		kpobject.h kplineobject.h kprectobject.h kpellipseobject.h \
		kpautoformobject.h kpclipartobject.h kptextobject.h \
		kppixmapobject.h kppieobject.h kppartobject.h kpgroupobject.h \
		kpbackground.h kpclipartcollection.h kpgradient.h \
		kpimage.h kpgradientcollection.h \
		gotopage.h webpresentation.h \
		footer_header.h kpresenter_factory.h kpresenter_aboutdata.h \
		KPresenterDocIface.h KPresenterViewIface.h KPresenterObjectIface.h \
		KPresenterBackgroundIface.h sidebar.h  kpresenter_dlg_config.h \
		kprvariable.h kprdrag.h notebar.h kprtextdocument.h kpfreehandobject.h \
		kppolylineobject.h kpquadricbeziercurveobject.h kpcubicbeziercurveobject.h \
		confpolygondia.h kppolygonobject.h

kdelnk_DATA = kpresenter.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kpresenter.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kpresenter
rc_DATA = kpresenter.rc

SUBDIRS = autoformEdit autoforms . pics templates dtd slideshow toolbar

messages: rc.cpp
	$(XGETTEXT) rc.cpp */*.cc *.cc *.h kpresenter_aboutdata.h -o $(podir)/kpresenter.pot
