INCLUDES= $(all_includes) -I$(srcdir)/..

datadir = $(kde_datadir)/killustrator

noinst_LIBRARIES = libkillu.a

noinst_HEADERS = Handle.h GObject.h GPolyline.h GPolygon.h GOval.h \
		 GText.h GGroup.h GClipart.h GBezier.h GPixmap.h \
		 Tool.h PolylineTool.h \
		 RectangleTool.h OvalTool.h TextTool.h FreeHandTool.h \
		 SelectionTool.h PolygonTool.h ToolController.h \
		 EditPointTool.h BezierTool.h ZoomTool.h \
		 ColorField.h Canvas.h \
		 QwViewport.h GDocument.h \
		 Coord.h PropertyEditor.h FloatSpinBox.h \
                 ColorComboBox.h FontSelector.h AlignmentDialog.h \
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
		 Ruler.h AboutDialog.h \
		 qwmf.h wmfstruct.h metafuncs.h version.h \
		 Arrow.h Painter.h LineStyle.h \
		 TransformationDialog.h koPageLayoutDia.h \
		 MainView.h PStateManager.h OptionDialog.h \
		 Preview.h units.h UnitBox.h ScriptDialog.h \
		 GLayer.h LayerDialog.h LayerView.h \
		 DocumentInfo.h Gradient.h GradientShape.h \
		 BrushCells.h PathTextTool.h \
		 EllipseConfigDialog.h MyTabCtl.h HelplineDialog.h \
		 GCurve.h ToCurveCmd.h BlendDialog.h BlendCmd.h \
		 StartupScreen.h

libkillu_a_SOURCES = ScriptDialog.cc Handle.cc GObject.cc GPolyline.cc GPolygon.cc \
			  PathTextTool.cc GOval.cc GClipart.cc GBezier.cc GPixmap.cc \
			  GText.cc GGroup.cc Tool.cc PolylineTool.cc \
			  FreeHandTool.cc \
			  RectangleTool.cc OvalTool.cc TextTool.cc \
			  SelectionTool.cc PolygonTool.cc ToolController.cc \
			  EditPointTool.cc BezierTool.cc ZoomTool.cc \
			  ColorField.cc Canvas.cc \
                          QwViewport.cc GDocument.cc \
			  Coord.cc PropertyEditor.cc FloatSpinBox.cc \
                          ColorComboBox.cc FontSelector.cc AlignmentDialog.cc \
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
			  Ruler.cc AboutDialog.cc qwmf.cc \
			  Arrow.cc Painter.cc LineStyle.cc \
			  TransformationDialog.cc koPageLayoutDia.cc \
			  PStateManager.cc OptionDialog.cc \
			  Preview.cc units.cc UnitBox.cc \
			  GLayer.cc LayerDialog.cc LayerView.cc \
			  DocumentInfo.cc Gradient.cc GradientShape.cc \
			  BrushCells.cc  \
			  EllipseConfigDialog.cc MyTabCtl.cc \
			  HelplineDialog.cc GCurve.cc ToCurveCmd.cc \
			  BlendDialog.cc BlendCmd.cc \
			  StartupScreen.cc

# just to make sure, automake makes them 
METASOURCES =	Handle.moc GObject.moc GPolyline.moc GPolygon.moc GOval.moc \
		GText.moc GGroup.moc GClipart.moc GBezier.moc GPixmap.moc \
		Tool.moc PolylineTool.moc FreeHandTool.moc \
		RectangleTool.moc OvalTool.moc TextTool.moc \
		SelectionTool.moc PolygonTool.moc ToolController.moc \
		EditPointTool.moc BezierTool.moc ZoomTool.moc \
		ColorField.moc Canvas.moc \
		QwViewport.moc GDocument.moc \
		PropertyEditor.moc FloatSpinBox.moc ColorComboBox.moc \
		FontSelector.moc AlignmentDialog.moc GridDialog.moc \
		PolygonConfigDialog.moc Ruler.moc AboutDialog.moc \
		PolygonPreview.moc TransformationDialog.moc \
		koPageLayoutDia.moc \
		PStateManager.moc OptionDialog.moc UnitBox.moc \
		ScriptDialog.moc GLayer.moc LayerDialog.moc \
		LayerView.moc CommandHistory.moc \
		DocumentInfo.moc BrushCells.moc PathTextTool.moc \
		EllipseConfigDialog.moc MyTabCtl.moc \
		HelplineDialog.moc GCurve.moc BlendDialog.moc \
		StartupScreen.moc



