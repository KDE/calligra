KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(kde_includes)/arts $(all_includes)

bin_SCRIPTS = kprconverter.pl

## The part
lib_LTLIBRARIES =
kde_module_LTLIBRARIES = libkpresenterpart.la
libkpresenterpart_la_SOURCES = koPointArray.cc kprcanvas.cc kprpage.cc searchdia.cc backdia.cc \
	webpresentation.cc kpresenter_view.cc  mspresentation.cc \
	kpresenter_doc.cc \
	styledia.cc kpresenter_sound_player.cc \
	pgconfdia.cc effectdia.cc kprcommand.cc \
	kpresenter_utils.cc  \
  effecthandler.cc pageeffects.cc\
	transeffectdia.cc \
	confpiedia.cc confrectdia.cc \
	kpobject.cc kplineobject.cc kprectobject.cc kpellipseobject.cc \
	kpautoformobject.cc kptextobject.cc \
	kppixmapobject.cc kppieobject.cc kppartobject.cc kpgroupobject.cc \
	kpbackground.cc kpgradient.cc \
	kpgradientcollection.cc \
	gotopage.cc \
	kpresenter_factory.cc \
	KPresenterDocIface.skel KPresenterDocIface.cc \
	KPresenterViewIface.skel KPresenterViewIface.cc\
	KPresenterObjectIface.skel KPresenterObjectIface.cc \
	KPresenterPageIface.skel KPresenterPageIface.cc \
	sidebar.cc \
	insertpagedia.ui generalconfig.ui \
	kpresenter_dlg_config.cc kprvariable.cc kprdrag.cc \
	kprtextdocument.cc kppointobject.cc kpfreehandobject.cc kppolylineobject.cc \
	kpquadricbeziercurveobject.cc kpcubicbeziercurveobject.cc \
	confpolygondia.cc kppolygonobject.cc notebar.cc \
	KPTextObjectIface.skel KPTextObjectIface.cc \
	KPTextViewIface.skel KPTextViewIface.cc presdurationdia.cc \
	KPPieObjectIface.skel KPPieObjectIface.cc \
	KPPolygonObjectIface.skel KPPolygonObjectIface.cc \
	KPRectObjectIface.skel KPRectObjectIface.cc \
	KPPixmapObjectIface.skel KPPixmapObjectIface.cc \
	KPAutoFormObjectIface.skel KPAutoFormObjectIface.cc \
	KPresenterObject2DIface.skel KPresenterObject2DIface.cc \
	KPLineObjectIface.skel KPLineObjectIface.cc \
	KPQuadricbezierCurveObjectIface.skel KPQuadricbezierCurveObjectIface.cc \
	KPCubicbeziercurveObjectIface.skel KPCubicbeziercurveObjectIface.cc \
	shadowdialog.ui shadowdialogimpl.cc textpreview.cc \
	rotationdialogimpl.cc rotationdialog.ui \
	KPFreehandObjectIface.skel KPFreehandObjectIface.cc kprstylemanager.cc \
	kprbgspellcheck.cc kprhelplinedia.cc confpicturedia.cc \
	KPPolyLineObjectIface.skel KPPolyLineObjectIface.cc \
	kprduplicateobjdia.cc kpclosedlineobject.cc \
	kprimportstyledia.cc \
	imageEffectDia.cc imageEffectBase.ui customslideshowdia.cc \
	slidetransitionwidget.ui slidetransitiondia.cc

if include_ARTS
ARTSKDE = -lartskde
endif

libkpresenterpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkpresenterpart_la_LIBADD = autoformEdit/libaf.la $(LIB_KOFFICEUI) $(LIB_KOTEXT) $(ARTSKDE) -lkdefx
libkpresenterpart_la_METASOURCES = AUTO

## The kdeinit loadable module and the executable
kdeinit_LTLIBRARIES = kpresenter.la
bin_PROGRAMS =
kpresenter_la_SOURCES = main.cc
kpresenter_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kpresenter_la_LIBADD = $(LIB_KOFFICECORE)


kdelnk_DATA = kpresenter.desktop
kdelnkdir = $(kde_appsdir)/Office

kde_services_DATA = kpresenterpart.desktop

rcdir = $(kde_datadir)/kpresenter
rc_DATA = kpresenter.rc kpresenter_readonly.rc

SUBDIRS = autoformEdit autoforms . pics templates dtd slideshow toolbar

messages: rc.cpp
	$(XGETTEXT) rc.cpp */*.cc *.cc *.h kpresenter_aboutdata.h -o $(podir)/kpresenter.pot
