# This file you have to edit. Change the name here
SUBDIRS = filter pics templates

INCLUDES  = $(KOFFICE_INCLUDES) -I$(srcdir)/filter -I$(srcdir) $(all_includes)

messages: rc.cpp
	$(XGETTEXT) -kI18N -kSI18N rc.cpp *.cc KIllustrator_aboutdata.h -o $(podir)/kontour.pot

kdelnk_DATA = killustrator.desktop kontour.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-killustrator.desktop x-kontour.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kontour
rc_DATA = KIllustrator.rc

## The kdeinit loadable module
lib_LTLIBRARIES = kontour.la
kontour_la_SOURCES = main.cc
kontour_la_LDFLAGS = $(all_libraries) -avoid-version -module
kontour_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kontour
kontour_SOURCES = dummy.cc
kontour_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kontour_LDADD = kontour.la

dummy.cc:
	echo > dummy.cc

## The part
kde_module_LTLIBRARIES = libkontourpart.la
libkontourpart_la_SOURCES = KIllustrator_doc.cc \
	KIllustrator_view.cc \
	InsertPartTool.cc GPart.cc KIllustrator_factory.cc \
	GPage.cc \
	tooldockmanager.cc \
	Handle.cc GObject.cc GPolyline.cc GPolygon.cc \
	PathTextTool.cc GOval.cc GClipart.cc GBezier.cc GPixmap.cc \
	GText.cc GGroup.cc Tool.cc PolylineTool.cc \
	FreeHandTool.cc \
	RectangleTool.cc OvalTool.cc TextTool.cc \
	SelectionTool.cc PolygonTool.cc ToolController.cc \
	EditPointTool.cc BezierTool.cc ZoomTool.cc \
	Canvas.cc GDocument.cc \
	Coord.cc PropertyEditor.cc AlignmentDialog.cc \
	PolygonConfigDialog.cc PolygonPreview.cc \
	Command.cc DuplicateCmd.cc \
	AlignCmd.cc DistributeCmd.cc InsertClipartCmd.cc \
	TranslateCmd.cc ScaleCmd.cc RotateCmd.cc \
	CreateOvalCmd.cc CreatePolylineCmd.cc \
	CreateTextCmd.cc CreateRectangleCmd.cc \
	GroupCmd.cc UngroupCmd.cc DeleteCmd.cc \
	CutCmd.cc CopyCmd.cc PasteCmd.cc \
	SetTextCmd.cc SetPropertyCmd.cc ReorderCmd.cc \
	AddLineSegmentCmd.cc EditPointCmd.cc \
	CreatePolygonCmd.cc CreateBezierCmd.cc \
	RemovePointCmd.cc InsertPointCmd.cc \
	TextAlongPathCmd.cc InsertPixmapCmd.cc \
	InsertObjCmd.cc ShearCmd.cc \
	SplitLineCmd.cc CommandHistory.cc \
	FilterInfo.cc ExportFilter.cc \
	ImportFilter.cc Ruler.cc \
	Arrow.cc Painter.cc LineStyle.cc \
	TransformationDialog.cc \
	PStateManager.cc OptionDialog.cc \
	units.cc UnitBox.cc \
	GLayer.cc LayerView.cc \
	Gradient.cc GradientShape.cc \
	BrushCells.cc EllipseConfigDialog.cc \
	GCurve.cc ToCurveCmd.cc \
	BlendDialog.cc BlendCmd.cc stencilbarmovemanager.cc \
	filter_utils.cc LayerPanel.cc TabBar.cc tooldockbase.cc

libkontourpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkontourpart_la_LIBADD = $(top_builddir)/kontour/filter/libfilter.la $(LIB_KOFFICEUI)

METASOURCES = AUTO
