KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

bin_SCRIPTS = kprconverter.pl

## The part
kde_module_LTLIBRARIES = libkpresenterpart.la
libkpresenterpart_la_SOURCES = backdia.cc webpresentation.cc kpresenter_view.cc \
	kpresenter_doc.cc \
	page.cc ktextedit.cc styledia.cc \
	qwmf.cc optiondia.cc pgconfdia.cc effectdia.cc \
	kpresenter_utils.cc rotatedia.cc searchdia.ui \
	shadowdia.cc presstructview.cc \
	confpiedia.cc confrectdia.cc \
	kpobject.cc kplineobject.cc kprectobject.cc kpellipseobject.cc \
	kpautoformobject.cc kpclipartobject.cc kptextobject.cc \
	kppixmapobject.cc kppieobject.cc kppartobject.cc kpgroupobject.cc \
	kpbackground.cc kpclipartcollection.cc kpgradient.cc \
	kpgradientcollection.cc \
	command.cc movecmd.cc resizecmd.cc insertcmd.cc deletecmd.cc \
	setoptionscmd.cc chgclipcmd.cc chgpixcmd.cc setbackcmd.cc \
	effectcmd.cc pgconfcmd.cc pglayoutcmd.cc shadowcmd.cc textcmd.cc \
	rotatecmd.cc lowraicmd.cc pievaluecmd.cc rectvaluecmd.cc penbrushcmd.cc \
	grpobjcmd.cc commandhistory.cc gotopage.cc footer_header.cc \
	kpresenter_factory.cc \
	KPresenterDocIface.skel KPresenterDocIface.cc \
	KPresenterViewIface.skel KPresenterViewIface.cc \
	KPresenterObjectIface.skel KPresenterObjectIface.cc \
	KPresenterBackgroundIface.skel KPresenterBackgroundIface.cc \
	textdialog.ui sidebar.cc \
	insertpagedia.ui generalconfig.ui kpimage.cc \
	preview.h preview.cc

libkpresenterpart_la_LDFLAGS = $(KDE_PLUGIN)
libkpresenterpart_la_LIBADD = autoformEdit/libaf.la ../lib/kofficeui/libkofficeui.la ../lib/koml/libkoml.la
libkpresenterpart_la_METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = kpresenter.la
kpresenter_la_SOURCES = main.cc
kpresenter_la_LDFLAGS = $(all_libraries) -avoid-version -module
kpresenter_la_LIBADD = ../lib/kofficecore/libkofficecore.la

## The executable
bin_PROGRAMS = kpresenter
kpresenter_SOURCES = dummy.cc
kpresenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kpresenter_LDADD = kpresenter.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = kpresenter_doc.h kpresenter_view.h \
		backdia.h page.h ktextedit.h styledia.h \
		qwmf.h global.h optiondia.h \
		pgconfdia.h effectdia.h kpresenter_utils.h \
		rotatedia.h shadowdia.h presstructview.h \
		confpiedia.h confrectdia.h \
		kpobject.h kplineobject.h kprectobject.h kpellipseobject.h \
		kpautoformobject.h kpclipartobject.h kptextobject.h \
		kppixmapobject.h kppieobject.h kppartobject.h kpgroupobject.h \
		kpbackground.h kpclipartcollection.h kpgradient.h \
		kpimage.h kpgradientcollection.h \
		command.h movecmd.h resizecmd.h insertcmd.h deletecmd.h \
		setoptionscmd.h chgclipcmd.h chgpixcmd.h setbackcmd.h \
		effectcmd.h pgconfcmd.h pglayoutcmd.h shadowcmd.h \
		rotatecmd.h lowraicmd.h pievaluecmd.h rectvaluecmd.h penbrushcmd.cc \
		grpobjcmd.h commandhistory.h gotopage.h webpresentation.h \
		footer_header.h kpresenter_factory.h kpresenter_aboutdata.h \
		KPresenterDocIface.h KPresenterViewIface.h KPresenterObjectIface.h \
		KPresenterBackgroundIface.h sidebar.h textcmd.h

kdelnk_DATA = kpresenter.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kpresenter.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kpresenter
rc_DATA = kpresenter.rc

SUBDIRS = autoformEdit autoforms . pics templates dtd slideshow toolbar

messages: rc.cpp
	$(XGETTEXT) rc.cpp */*.cc *.cc *.h kpresenter_aboutdata.h -o $(podir)/kpresenter.pot
