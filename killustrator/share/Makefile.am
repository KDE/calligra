INCLUDES= $(all_includes) -I.. 

datadir = $(kde_datadir)/killustrator

noinst_LIBRARIES = libkillu.a

noinst_HEADERS = Handle.h GObject.h GPolyline.h GPolygon.h GOval.h \
		 GText.h GGroup.h GClipart.h GBezier.h Tool.h PolylineTool.h \
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
		 GroupCmd.h UngroupCmd.h DeleteCmd.h \
		 CutCmd.h CopyCmd.h PasteCmd.h SetTextCmd.h \
		 SetPropertyCmd.h ReorderCmd.h \
		 AddLineSegmentCmd.h EditPointCmd.h \
		 CreatePolygonCmd.h CreateBezierCmd.h \
		 InsertObjCmd.h ShearCmd.h CommandHistory.h \
		 FilterInfo.h ExportFilter.h ImportFilter.h \
		 Ruler.h AboutDialog.h \
		 qwmf.h wmfstruct.h metafuncs.h version.h \
		 Arrow.h Painter.h LineStyle.h \
		 TransformationDialog.h koPageLayoutDia.h \
		 MainView.h PStateManager.h OptionDialog.h \
		 Preview.h units.h UnitBox.h ScriptDialog.h \
		 GLayer.h LayerDialog.h LayerView.h \
		 DocumentInfo.h Gradient.h GradientShape.h \
		 BrushCells.h PathTextTool.h

libkillu_a_SOURCES = Handle.cc GObject.cc GPolyline.cc GPolygon.cc \
			  GOval.cc GClipart.cc GBezier.cc \
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
			  GridDialog.cc Command.cc \
                          AlignCmd.cc DistributeCmd.cc InsertClipartCmd.cc \
			  TranslateCmd.cc ScaleCmd.cc RotateCmd.cc \
			  CreateOvalCmd.cc CreatePolylineCmd.cc \
			  CreateTextCmd.cc CreateRectangleCmd.cc \
			  GroupCmd.cc UngroupCmd.cc DeleteCmd.cc \
			  CutCmd.cc CopyCmd.cc PasteCmd.cc \
			  SetTextCmd.cc SetPropertyCmd.cc ReorderCmd.cc \
			  AddLineSegmentCmd.cc EditPointCmd.cc \
			  CreatePolygonCmd.cc CreateBezierCmd.cc \
			  InsertObjCmd.cc ShearCmd.cc CommandHistory.cc \
			  FilterInfo.cc ExportFilter.cc \
			  ImportFilter.cc \
			  Ruler.cc AboutDialog.cc qwmf.cc \
			  Arrow.cc Painter.cc LineStyle.cc \
			  TransformationDialog.cc koPageLayoutDia.cc \
			  PStateManager.cc OptionDialog.cc \
			  Preview.cc units.cc UnitBox.cc ScriptDialog.cc \
			  GLayer.cc LayerDialog.cc LayerView.cc \
			  DocumentInfo.cc Gradient.cc GradientShape.cc \
			  BrushCells.cc PathTextTool.cc

# just to make sure, automake makes them 
BUILTSOURCES =	Handle.moc GObject.moc GPolyline.moc GPolygon.moc GOval.moc \
		GText.moc GGroup.moc GClipart.moc GBezier.moc \
		Tool.moc PolylineTool.moc FreeHandTool.moc \
		RectangleTool.moc OvalTool.moc TextTool.moc \
		SelectionTool.moc PolygonTool.moc ToolController.moc \
		EditPointTool.moc BezierTool.moc ZoomTool.moc \
		ColorField.moc Canvas.moc \
		QwViewport.moc GDocument.moc KIllustrator.moc \
		PropertyEditor.moc FloatSpinBox.moc ColorComboBox.moc \
		FontSelector.moc AlignmentDialog.moc GridDialog.moc \
		PolygonConfigDialog.moc KIllustratorApp.moc \
		Ruler.moc AboutDialog.moc \
		PolygonPreview.moc \
		TransformationDialog.moc koPageLayoutDia.moc \
		PStateManager.moc OptionDialog.moc UnitBox.moc \
		ScriptDialog.moc GLayer.moc LayerDialog.moc \
		LayerView.moc CommandHistory.moc \
		DocumentInfo.moc BrushCells.moc PathTextTool.moc

DISTCLEANFILES = $(BUILTSOURCES)

SUFFIXES = .moc .h

.h.moc:
	$(MOC) $< -o $@  

findmoc:
	grep moc $(libkillu_a_SOURCES) | sed -e "s/#include \"/ /; s/\"\$$//"

Handle.cc: Handle.moc
GObject.cc: GObject.moc
GPolyline.cc: GPolyline.moc
GPolygon.cc: GPolygon.moc
GOval.cc: GOval.moc
GClipart.cc: GClipart.moc
GBezier.cc: GBezier.moc
GText.cc: GText.moc
GGroup.cc: GGroup.moc
Tool.cc: Tool.moc
PolylineTool.cc: PolylineTool.moc
FreeHandTool.cc: FreeHandTool.moc
RectangleTool.cc: RectangleTool.moc
OvalTool.cc: OvalTool.moc
TextTool.cc: TextTool.moc
SelectionTool.cc: SelectionTool.moc
PolygonTool.cc: PolygonTool.moc
ToolController.cc: ToolController.moc
EditPointTool.cc: EditPointTool.moc
BezierTool.cc: BezierTool.moc
ZoomTool.cc: ZoomTool.moc
ColorField.cc: ColorField.moc
Canvas.cc: Canvas.moc
QwViewport.cc: QwViewport.moc
GDocument.cc: GDocument.moc
PropertyEditor.cc: PropertyEditor.moc
FloatSpinBox.cc: FloatSpinBox.moc
ColorComboBox.cc: ColorComboBox.moc
FontSelector.cc: FontSelector.moc
AlignmentDialog.cc: AlignmentDialog.moc
PolygonConfigDialog.cc: PolygonConfigDialog.moc
PolygonPreview.cc: PolygonPreview.moc
GridDialog.cc: GridDialog.moc
CommandHistory.cc: CommandHistory.moc
Ruler.cc: Ruler.moc
AboutDialog.cc: AboutDialog.moc
TransformationDialog.cc: TransformationDialog.moc
koPageLayoutDia.cc: koPageLayoutDia.moc
PStateManager.cc: PStateManager.moc
OptionDialog.cc: OptionDialog.moc
UnitBox.cc: UnitBox.moc
ScriptDialog.cc: ScriptDialog.moc
GLayer.cc: GLayer.moc
LayerDialog.cc: LayerDialog.moc
LayerView.cc: LayerView.moc
DocumentInfo.cc: DocumentInfo.moc
BrushCells.cc: BrushCells.moc
PathTextTool.cc: PathTextTool.moc


