INCLUDES= -I$(srcdir)/.. -I$(srcdir)/../../lib/kofficeui $(all_includes)

datadir = $(kde_datadir)/killustrator

noinst_LTLIBRARIES = libkillu.la

noinst_HEADERS = Handle.h GObject.h GPolyline.h GPolygon.h GOval.h \
		 GText.h GGroup.h GClipart.h GBezier.h GPixmap.h \
		 Tool.h PolylineTool.h \
		 RectangleTool.h OvalTool.h TextTool.h FreeHandTool.h \
		 SelectionTool.h PolygonTool.h ToolController.h \
		 EditPointTool.h BezierTool.h ZoomTool.h \
		 Canvas.h \
		 GDocument.h \
		 Coord.h PropertyEditor.h \
                             AlignmentDialog.h \
		 GridDialog.h PolygonConfigDialog.h PolygonPreview.h \
		 Command.h AlignCmd.h DistributeCmd.h InsertClipartCmd.h \
		 TranslateCmd.h ScaleCmd.h RotateCmd.h \
		 CreateOvalCmd.h CreatePolylineCmd.h \
		 CreateTextCmd.h CreateRectangleCmd.h \
		 GroupCmd.h UngroupCmd.h DeleteCmd.h DuplicateCmd.h \
		 CutCmd.h CopyCmd.h PasteCmd.h SetTextCmd.h \
		 SetPropertyCmd.h ReorderCmd.h \
		 AddLineSegmentCmd.h EditPointCmd.h \
		 CreatePolygonCmd.h CreateBezierCmd.h \
		 RemovePointCmd.h InsertPointCmd.h TextAlongPathCmd.h \
		 InsertObjCmd.h ShearCmd.h InsertPixmapCmd.h \
		 SplitLineCmd.h CommandHistory.h \
		 FilterInfo.h ExportFilter.h ImportFilter.h \
		 Ruler.h \
		 qwmf.h wmfstruct.h metafuncs.h \
		 Arrow.h Painter.h LineStyle.h \
		 TransformationDialog.h \
		 MainView.h PStateManager.h OptionDialog.h \
		 units.h UnitBox.h \
		 GLayer.h LayerDialog.h LayerView.h \
		 Gradient.h GradientShape.h \
		 BrushCells.h PathTextTool.h \
		 EllipseConfigDialog.h HelplineDialog.h \
		 GCurve.h ToCurveCmd.h BlendDialog.h BlendCmd.h \
		 filter_utils.h CanvasView.h

libkillu_la_SOURCES = Handle.cc GObject.cc GPolyline.cc GPolygon.cc \
			  PathTextTool.cc GOval.cc GClipart.cc GBezier.cc GPixmap.cc \
			  GText.cc GGroup.cc Tool.cc PolylineTool.cc \
			  FreeHandTool.cc \
			  RectangleTool.cc OvalTool.cc TextTool.cc \
			  SelectionTool.cc PolygonTool.cc ToolController.cc \
			  EditPointTool.cc BezierTool.cc ZoomTool.cc \
			  Canvas.cc \
                                            GDocument.cc \
			  Coord.cc PropertyEditor.cc \
                                            AlignmentDialog.cc \
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
			  ImportFilter.cc \
			  Ruler.cc qwmf.cc \
			  Arrow.cc Painter.cc LineStyle.cc \
			  TransformationDialog.cc \
			  PStateManager.cc OptionDialog.cc \
			  units.cc UnitBox.cc \
			  GLayer.cc LayerDialog.cc LayerView.cc \
			  Gradient.cc GradientShape.cc \
			  BrushCells.cc  \
			  EllipseConfigDialog.cc \
			  HelplineDialog.cc GCurve.cc ToCurveCmd.cc \
			  BlendDialog.cc BlendCmd.cc \
			  filter_utils.cc CanvasView.cc

METASOURCES =	AUTO
