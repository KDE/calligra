# This file you have to edit. Change the name here
SUBDIRS = pics

INCLUDES  = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) -I$(srcdir) $(all_includes)

messages: rc.cpp
	$(XGETTEXT) -kI18N -kSI18N rc.cpp *.cc kontour_about.h -o $(podir)/kontour.pot

kdelnk_DATA = kontour.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kontour.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kontour
rc_DATA = kontour.rc

## The kdeinit loadable module
lib_LTLIBRARIES = kontour.la
kontour_la_SOURCES = main.cc
kontour_la_LDFLAGS = $(all_libraries) -avoid-version -module
kontour_la_LIBADD = ../lib/kofficecore/libkofficecore.la

## The executable
bin_PROGRAMS = kontour
kontour_SOURCES = dummy.cc
kontour_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kontour_LDADD = kontour.la

dummy.cc:
	echo > dummy.cc

## The part
kde_module_LTLIBRARIES = libkontourpart.la
libkontourpart_la_SOURCES = \
	kontour_view.cc \
	kontour_doc.cc \
	kontour_factory.cc \
	Canvas.cc \
	TabBar.cc \
	Ruler.cc \
	GDocument.cc \
	GPage.cc \
	GLayer.cc \
	GObject.cc \
	GPath.cc \
	GOval.cc \
	GRect.cc \
	GStyle.cc \
	GStyleList.cc \
	LayerPanel.cc \
	StylePanel.cc \
	Tool.cc \
	ToolController.cc \
	SelectTool.cc \
	RectTool.cc \
	OvalTool.cc \
	ZoomTool.cc \
	Handle.cc \
	Command.cc \
	CopyCmd.cc \
	CutCmd.cc \
	PasteCmd.cc \
	DeleteCmd.cc \
	DuplicateCmd.cc \
	ToPathCmd.cc \
	TranslateCmd.cc \
	RotateCmd.cc \
	ScaleCmd.cc \
	ShearCmd.cc \
	CreateOvalCmd.cc \
	CreateRectCmd.cc \
	OptionsDialog.cc \
	UnitBox.cc \
	units.cc

libkontourpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkontourpart_la_LIBADD = $(top_builddir)/lib/kofficeui/libkofficeui.la $(LIB_KOPAINTER)

METASOURCES = AUTO
