INCLUDES= $(all_includes) -I.. 

noinst_LIBRARIES = libkillu.a

noinst_HEADERS = Handle.h GObject.h GPolyline.h GPolygon.h GOval.h \
		 GText.h GGroup.h GClipart.h GBezier.h Tool.h PolylineTool.h \
		 RectangleTool.h OvalTool.h TextTool.h FreeHandTool.h \
		 SelectionTool.h PolygonTool.h ToolController.h \
		 EditPointTool.h BezierTool.h ZoomTool.h \
		 ToolGroup.h ToolPalette.h ColorField.h Canvas.h \
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
		 InsertObjCmd.h CommandHistory.h \
		 FilterInfo.h ExportFilter.h ImportFilter.h \
		 Ruler.h AboutDialog.h \
		 qwmf.h wmfstruct.h metafuncs.h version.h \
		 ToolButton.h Arrow.h Painter.h LineStyle.h \
		 TransformationDialog.h koPageLayoutDia.h \
		 MainView.h PStateManager.h OptionDialog.h \
		 Preview.h

libkillu_a_SOURCES = Handle.cc GObject.cc GPolyline.cc GPolygon.cc \
			  GOval.cc GClipart.cc GBezier.cc \
			  GText.cc GGroup.cc Tool.cc PolylineTool.cc FreeHandTool.cc \
			  RectangleTool.cc OvalTool.cc TextTool.cc \
			  SelectionTool.cc PolygonTool.cc ToolController.cc \
			  EditPointTool.cc BezierTool.cc ZoomTool.cc \
			  ToolGroup.cc ToolPalette.cc ColorField.cc Canvas.cc \
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
			  InsertObjCmd.cc CommandHistory.cc \
			  FilterInfo.cc ExportFilter.cc \
			  ImportFilter.cc \
			  Ruler.cc AboutDialog.cc qwmf.cc \
			  ToolButton.cc Arrow.cc Painter.cc LineStyle.cc \
			  TransformationDialog.cc koPageLayoutDia.cc \
			  PStateManager.cc OptionDialog.cc \
			  Preview.cc

# just to make sure, automake makes them 
BUILTSOURCES =	Handle.moc GObject.moc GPolyline.moc GPolygon.moc GOval.moc \
		GText.moc GGroup.moc GClipart.moc GBezier.moc \
		Tool.moc PolylineTool.moc FreeHandTool.moc \
		RectangleTool.moc OvalTool.moc TextTool.moc \
		SelectionTool.moc PolygonTool.moc ToolController.moc \
		EditPointTool.moc BezierTool.moc ZoomTool.moc \
		ToolGroup.moc ToolPalette.moc ColorField.moc Canvas.moc \
		QwViewport.moc GDocument.moc KIllustrator.moc \
		PropertyEditor.moc FloatSpinBox.moc ColorComboBox.moc \
		FontSelector.moc AlignmentDialog.moc GridDialog.moc \
		PolygonConfigDialog.moc KIllustratorApp.moc \
		Ruler.moc AboutDialog.moc \
		ToolButton.moc PolygonPreview.moc \
		TransformationDialog.moc koPageLayoutDia.moc \
		PStateManager.moc OptionDialog.moc

DISTCLEANFILES = $(BUILTSOURCES)

# add a dependency for every moc file to be full portable
# I've added a key binding to emacs for this. 
Handle.cc: Handle.moc
Handle.moc: Handle.h
	$(MOC) Handle.h -o Handle.moc

GObject.cc: GObject.moc
GObject.moc: GObject.h
	$(MOC) GObject.h -o GObject.moc

GPolyline.cc: GPolyline.moc
GPolyline.moc: GPolyline.h
	$(MOC) GPolyline.h -o GPolyline.moc

GPolygon.cc: GPolygon.moc
GPolygon.moc: GPolygon.h
	$(MOC) GPolygon.h -o GPolygon.moc

GOval.cc: GOval.moc
GOval.moc: GOval.h
	$(MOC) GOval.h -o GOval.moc

GText.cc: GText.moc
GText.moc: GText.h
	$(MOC) GText.h -o GText.moc

GGroup.cc: GGroup.moc
GGroup.moc: GGroup.h
	$(MOC) GGroup.h -o GGroup.moc

Tool.cc: Tool.moc
Tool.moc: Tool.h
	$(MOC) Tool.h -o Tool.moc

RectangleTool.cc: RectangleTool.moc
RectangleTool.moc: RectangleTool.h
	$(MOC) RectangleTool.h -o RectangleTool.moc

PolylineTool.cc: PolylineTool.moc
PolylineTool.moc: PolylineTool.h
	$(MOC) PolylineTool.h -o PolylineTool.moc

FreeHandTool.cc: FreeHandTool.moc
FreeHandTool.moc: FreeHandTool.h
	$(MOC) FreeHandTool.h -o FreeHandTool.moc

OvalTool.cc: OvalTool.moc
OvalTool.moc: OvalTool.h
	$(MOC) OvalTool.h -o OvalTool.moc

TextTool.cc: TextTool.moc
TextTool.moc: TextTool.h
	$(MOC) TextTool.h -o TextTool.moc

SelectionTool.cc: SelectionTool.moc
SelectionTool.moc: SelectionTool.h
	$(MOC) SelectionTool.h -o SelectionTool.moc

EditPointTool.cc: EditPointTool.moc
EditPointTool.moc: EditPointTool.h
	$(MOC) EditPointTool.h -o EditPointTool.moc

BezierTool.cc: BezierTool.moc
BezierTool.moc: BezierTool.h
	$(MOC) BezierTool.h -o BezierTool.moc

ZoomTool.cc: ZoomTool.moc
ZoomTool.moc: ZoomTool.h
	$(MOC) ZoomTool.h -o ZoomTool.moc

ToolController.cc: ToolController.moc
ToolController.moc: ToolController.h
	$(MOC) ToolController.h -o ToolController.moc

ToolPalette.cc: ToolPalette.moc
ToolPalette.moc: ToolPalette.h
	$(MOC) ToolPalette.h -o ToolPalette.moc

ToolGroup.cc: ToolGroup.moc
ToolGroup.moc: ToolGroup.h
	$(MOC) ToolGroup.h -o ToolGroup.moc

ColorField.cc: ColorField.moc
ColorField.moc: ColorField.h
	$(MOC) ColorField.h -o ColorField.moc

Canvas.cc: Canvas.moc
Canvas.moc: Canvas.h
	$(MOC) Canvas.h -o Canvas.moc

QwViewport.cc: QwViewport.moc
QwViewport.moc: QwViewport.h
	$(MOC) QwViewport.h -o QwViewport.moc

GDocument.cc: GDocument.moc
GDocument.moc: GDocument.h
	$(MOC) GDocument.h -o GDocument.moc

PropertyEditor.cc: PropertyEditor.moc
PropertyEditor.moc: PropertyEditor.h
	$(MOC) PropertyEditor.h -o PropertyEditor.moc

FloatSpinBox.cc: FloatSpinBox.moc
FloatSpinBox.moc: FloatSpinBox.h
	$(MOC) FloatSpinBox.h -o FloatSpinBox.moc

ColorComboBox.cc: ColorComboBox.moc
ColorComboBox.moc: ColorComboBox.h
	$(MOC) ColorComboBox.h -o ColorComboBox.moc

FontSelector.cc: FontSelector.moc
FontSelector.moc: FontSelector.h
	$(MOC) FontSelector.h -o FontSelector.moc

AlignmentDialog.cc: AlignmentDialog.moc
AlignmentDialog.moc: AlignmentDialog.h
	$(MOC) AlignmentDialog.h -o AlignmentDialog.moc

Ruler.cc: Ruler.moc
Ruler.moc: Ruler.h
	$(MOC) Ruler.h -o Ruler.moc

AboutDialog.cc: AboutDialog.moc
AboutDialog.moc: AboutDialog.h
	$(MOC) AboutDialog.h -o AboutDialog.moc

GClipart.cc: GClipart.moc
GClipart.moc: GClipart.h
	$(MOC) GClipart.h -o GClipart.moc

GBezier.cc: GBezier.moc
GBezier.moc: GBezier.h
	$(MOC) GBezier.h -o GBezier.moc

GridDialog.cc: GridDialog.moc
GridDialog.moc: GridDialog.h
	$(MOC) GridDialog.h -o GridDialog.moc

ToolButton.cc: ToolButton.moc
ToolButton.moc: ToolButton.h
	$(MOC) ToolButton.h -o ToolButton.moc

PolygonTool.cc: PolygonTool.moc
PolygonTool.moc: PolygonTool.h
	$(MOC) PolygonTool.h -o PolygonTool.moc

PolygonPreview.cc: PolygonPreview.moc
PolygonPreview.moc: PolygonPreview.h
	$(MOC) PolygonPreview.h -o PolygonPreview.moc

PolygonConfigDialog.cc: PolygonConfigDialog.moc
PolygonConfigDialog.moc: PolygonConfigDialog.h
	$(MOC) PolygonConfigDialog.h -o PolygonConfigDialog.moc

TransformationDialog.cc: TransformationDialog.moc
TransformationDialog.moc: TransformationDialog.h
	$(MOC) TransformationDialog.h -o TransformationDialog.moc

koPageLayoutDia.cc: koPageLayoutDia.moc
koPageLayoutDia.moc: koPageLayoutDia.h
	$(MOC) koPageLayoutDia.h -o koPageLayoutDia.moc

PStateManager.cc: PStateManager.moc
PStateManager.moc: PStateManager.h
	$(MOC) PStateManager.h -o PStateManager.moc

OptionDialog.cc: OptionDialog.moc
OptionDialog.moc: OptionDialog.h
	$(MOC) OptionDialog.h -o OptionDialog.moc

