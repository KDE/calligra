# This file you have to edit. Change the name here
SUBDIRS = filter pics templates

INCLUDES  = $(KOFFICE_INCLUDES) -I$(top_srcdir)/killustrator/filter -I$(top_srcdir)/killustrator $(all_includes)

#EXTRA_DIST = README kil.dtd killustrator.lsm

messages:
	$(EXTRACTRC) *.rc >> rc.cpp
	$(XGETTEXT) -kI18N -kSI18N rc.cpp *.cc -o $(podir)/killustrator.pot

kdelnk_DATA = killustrator.desktop
kdelnkdir = $(kde_appsdir)/Office
 
kdemime_DATA = x-killustrator.desktop
kdemimedir = $(kde_mimedir)/application
 
rcdir = $(kde_datadir)/killustrator
rc_DATA = KIllustrator.rc

lib_LTLIBRARIES = libkillustratorpart.la killustrator.la

## The kdeinit loadable module
killustrator_la_SOURCES = main.cc
killustrator_la_LDFLAGS = $(all_libraries) -avoid-version -module
killustrator_la_LIBADD = libkillustratorpart.la

## The executable
bin_PROGRAMS = killustrator
killustrator_SOURCES = dummy.cc
killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)
killustrator_LDADD = killustrator.la

dummy.cc:
	echo > dummy.cc

## The part
libkillustratorpart_la_SOURCES = KIllustrator_doc.cc \
        KIllustrator_view.cc \
        InsertPartTool.cc GPart.cc KIllustrator_factory.cc \
	GPage.cc PageNameDialog.cc \
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
       	GridDialog.cc Command.cc DuplicateCmd.cc \
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
       	ImportFilter.cc Ruler.cc qwmf.cc \
       	Arrow.cc Painter.cc LineStyle.cc \
       	TransformationDialog.cc \
       	PStateManager.cc OptionDialog.cc \
       	units.cc UnitBox.cc \
       	GLayer.cc LayerView.cc \
       	Gradient.cc GradientShape.cc \
       	BrushCells.cc EllipseConfigDialog.cc \
       	HelplineDialog.cc GCurve.cc ToCurveCmd.cc \
       	BlendDialog.cc BlendCmd.cc stencilbarmovemanager.cc \
       	filter_utils.cc LayerPanel.cc TabBar.cc tooldockbase.cc

libkillustratorpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkillustratorpart_la_LIBADD = $(top_builddir)/killustrator/filter/libfilter.la $(top_builddir)/lib/kofficeui/libkofficeui.la

METASOURCES = AUTO

