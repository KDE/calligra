/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <iostream.h>

#include <qaccel.h>
#include "KIllustrator_shell.h"
#include "KIllustrator_view.h"
#include "KIllustrator_doc.h"
#include "KIllustrator_factory.h"
#include "MainView.h"

#include <qscrollview.h>
#include "GDocument.h"
#include "Canvas.h"
#include "Tool.h"
#include "Ruler.h"
#include "ToolController.h"
#include "RectangleTool.h"
#include "PolylineTool.h"
#include "FreeHandTool.h"
#include "SelectionTool.h"
#include "OvalTool.h"
#include "TextTool.h"
#include "PolygonTool.h"
#include "EditPointTool.h"
#include "BezierTool.h"
#include "ZoomTool.h"
#include "PathTextTool.h"
#include "InsertPartTool.h"
#include "PropertyEditor.h"
#include "AlignmentDialog.h"
#include "GridDialog.h"
#include "HelplineDialog.h"
#include "TransformationDialog.h"
#include "LayerDialog.h"
#include "PStateManager.h"
#include "ExportFilter.h"
#include "GroupCmd.h"
#include "UngroupCmd.h"
#include "DeleteCmd.h"
#include "CutCmd.h"
#include "CopyCmd.h"
#include "PasteCmd.h"
#include "DuplicateCmd.h"
#include "ReorderCmd.h"
#include "InsertClipartCmd.h"
#include "InsertPixmapCmd.h"
#include "SetPropertyCmd.h"
#include "FilterManager.h"
#include "ToCurveCmd.h"
#include "BlendCmd.h"
#include "BlendDialog.h"
#include "OptionDialog.h"
#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <qfileinfo.h>

#include <koPartSelectDia.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <kcoloractions.h>
#include <kmessagebox.h>

KIllustratorView::KIllustratorView (QWidget* parent, const char* name,
                                    KIllustratorDocument* doc) :
    KoView( doc, parent, name )
{
    setInstance( KIllustratorFactory::global() );
    setXMLFile( "KIllustrator.rc" );
    m_pDoc = doc;
    m_bShowGUI = true;
    m_bShowRulers = true;
    scrollview = 0L;
    layerDialog = 0L;
    objMenu = 0L;
    // restore default settings
    PStateManager::instance ();

    //kdDebug() << "connect doc" << endl;
    /*
      QObject::connect (m_pDoc,
      SIGNAL (partInserted (KIllustratorChild *, GPart *)),
      this,
      SLOT (insertPartSlot (KIllustratorChild *, GPart *)));
      QObject::connect (m_pDoc,
      SIGNAL (childGeometryChanged (KIllustratorChild *)),
      this, SLOT(changeChildGeometrySlot (KIllustratorChild *)));
    */
    createMyGUI();
}

KIllustratorView::~KIllustratorView()
{
    //kdDebug() << "~KIllustratorView ()" << endl;
}

void KIllustratorView::createMyGUI()
{
    setupCanvas ();

    // File menu
    m_import = new KAction( i18n("&Import..."), 0, this, SLOT( slotImport() ), actionCollection(), "import" );
    m_export = new KAction( i18n("&Export..."), 0, this, SLOT( slotExport() ), actionCollection(), "export" );

    // Insert menu
    m_insertBitmap = new KAction( i18n("Insert &Bitmap..."), 0, this, SLOT( slotInsertBitmap() ), actionCollection(), "insertBitmap" );
    m_insertClipart = new KAction( i18n("Insert &Clipart..."), 0, this, SLOT( slotInsertClipart() ), actionCollection(), "insertClipart" );

    // Edit menu
    m_copy = KStdAction::copy(this, SLOT( slotCopy() ), actionCollection(), "copy" );
    m_paste = KStdAction::paste(this, SLOT( slotPaste() ), actionCollection(), "paste" );
    m_cut = KStdAction::cut(this, SLOT( slotCut() ), actionCollection(), "cut" );
    m_undo = KStdAction::undo(this, SLOT( slotUndo() ), actionCollection(), "undo" );
    m_redo = KStdAction::redo(this, SLOT( slotRedo() ), actionCollection(), "redo" );
    m_duplicate = new KAction( i18n("Dup&licate"), 0, this, SLOT( slotDuplicate() ), actionCollection(), "duplicate" );
    m_delete = new KAction( i18n("&Delete"), 0, this, SLOT( slotDelete() ), actionCollection(), "delete" );
    m_selectAll = new KAction( i18n("&Select All"), 0, this, SLOT( slotSelectAll() ), actionCollection(), "selectAll" );
    m_properties = new KAction( i18n("&Properties..."), 0, this, SLOT( slotProperties() ), actionCollection(), "properties" );

    // View menu
    m_outline = new KToggleAction( i18n("Ou&tline"), 0, actionCollection(), "outline" );
    m_outline->setExclusiveGroup( "Outline" );
    connect( m_outline, SIGNAL( toggled( bool ) ), this, SLOT( slotOutline( bool ) ) );
    m_normal = new KToggleAction( i18n("&Normal"), 0, actionCollection(), "normal" );
    m_normal->setExclusiveGroup( "Outline" );
    connect( m_normal, SIGNAL( toggled( bool ) ), this, SLOT( slotNormal( bool ) ) );
    m_layers = new KAction( i18n("&Layers ..."), 0, this, SLOT( slotLayers() ), actionCollection(), "layers" );
    m_showRuler = new KToggleAction( i18n("Show &Ruler"), 0, actionCollection(), "showRuler" );
    connect( m_showRuler, SIGNAL( toggled( bool ) ), this, SLOT( slotShowRuler( bool ) ) );
    m_showGrid = new KToggleAction( i18n("Show &Grid"), 0, actionCollection(), "showGrid" );
    connect( m_showGrid, SIGNAL( toggled( bool ) ), this, SLOT( slotShowGrid( bool ) ) );
    m_showHelplines = new KToggleAction( i18n("Show &Helplines"), 0, actionCollection(), "showHelplines" );
    connect( m_showHelplines, SIGNAL( toggled( bool ) ), this, SLOT( slotShowHelplines( bool ) ) );

    // Layout menu
    m_page = new KAction( i18n("&Page ..."), 0, this, SLOT( slotPage() ), actionCollection(), "page" );
    m_grid = new KAction( i18n("&Grid ..."), 0, this, SLOT( slotGrid() ), actionCollection(), "grid" );
    m_helplines = new KAction( i18n("&Helplines ..."), 0, this, SLOT( slotHelplines() ), actionCollection(), "helplines" );
    m_alignToGrid = new KToggleAction( i18n("&Align To Grid"), 0, actionCollection(), "alignToGrid" );
    connect( m_alignToGrid, SIGNAL( toggled( bool ) ), this, SLOT( slotAlignToGrid( bool ) ) );
    m_alignToHelplines = new KToggleAction( i18n("Align &To Helplines"), 0, actionCollection(), "alignToHelplines" );
    connect( m_alignToHelplines, SIGNAL( toggled( bool ) ), this, SLOT( slotAlignToHelplines( bool ) ) );

    // Transform menu
    m_transformPosition = new KAction( i18n("&Position ..."), 0, this, SLOT( slotTransformPosition() ), actionCollection(), "transformPosition" );
    m_transformDimension = new KAction( i18n("&Dimension ..."), 0, this, SLOT( slotTransformDimension() ), actionCollection(), "transformDimension" );
    m_transformRotation = new KAction( i18n("&Rotation ..."), 0, this, SLOT( slotTransformRotation() ), actionCollection(), "transformRotation" );
    m_transformMirror = new KAction( i18n("&Mirror ..."), 0, this, SLOT( slotTransformMirror() ), actionCollection(), "transformMirror" );

    // Arrange menu
    m_distribute = new KAction( i18n("&Align/Distribute ..."), 0, this, SLOT( slotDistribute() ), actionCollection(), "distribute" );
    m_toFront = new KAction( i18n("To &Front"), 0, this, SLOT( slotToFront() ), actionCollection(), "toFront" );
    m_toBack = new KAction( i18n("To &Back"), 0, this, SLOT( slotToBack() ), actionCollection(), "toBack" );
    m_forwardOne = new KAction( i18n("Forward &One"), 0, this, SLOT( slotForwardOne() ), actionCollection(), "forwardOne" );
    m_backOne = new KAction( i18n("B&ack One"), 0, this, SLOT( slotBackOne() ), actionCollection(), "backOne" );
    m_group = new KAction( i18n("&Group"), 0, this, SLOT( slotGroup() ), actionCollection(), "group" );
    m_ungroup = new KAction( i18n("&Ungroup"), 0, this, SLOT( slotUngroup() ), actionCollection(), "ungroup" );
    m_textAlongPath = new KAction( i18n("Text Along &Path"), 0, this, SLOT( slotTextAlongPath() ), actionCollection(), "textAlongPath" );
    m_convertToCurve = new KAction( i18n("&Convert to Curve"), 0, this, SLOT( slotConvertToCurve() ), actionCollection(), "convertToCurve" );

    // Effects menu
    m_blend = new KAction( i18n("&Blend ..."), 0, this, SLOT( slotBlend() ), actionCollection(), "blend" );

    // Extras menu
    m_loadPalette = new KAction( i18n("&Load Palette ..."), 0, this, SLOT( slotLoadPalette() ), actionCollection(), "loadPalette" );
    m_options = new KAction( i18n("&Options ..."), 0, this, SLOT( slotOptions() ), actionCollection(), "options" );

    //
    m_viewZoom = new KSelectAction (i18n ("&Zoom"), 0, actionCollection (),
                                    "view_zoom");
    QStringList zooms;
    zooms << "50%";
    zooms << "100%";
    zooms << "150%";
    zooms << "200%";
    zooms << "400%";
    zooms << "600%";
    zooms << "800%";
    zooms << "1000%";

    ((KSelectAction *) m_viewZoom)->setItems (zooms);
    connect (((KSelectAction *) m_viewZoom),
             SIGNAL(activated(const QString &)),
             this, SLOT(slotViewZoom(const QString &)));
    ((KSelectAction *) m_viewZoom)->setCurrentItem (2);
    // Colorbar action

    QValueList<QColor> colorList;
    colorList << white << red << green << blue << cyan << magenta << yellow
              << darkRed << darkGreen << darkBlue << darkCyan
              << darkMagenta << darkYellow << white << lightGray
              << gray << darkGray << black;

    m_colorBar = new KColorBarAction( i18n( "&Colorbar" ), 0,
                                      this,
                                      SLOT( slotBrushChosen( const QColor & ) ),
                                      SLOT( slotPenChosen( const QColor & ) ),
                                      colorList,
                                      actionCollection(), "colorbar" );

    // Tools
    m_selectTool = new KToggleAction( i18n("Select Tool"), "selecttool", 0, actionCollection(), "selectTool" );
    m_selectTool->setExclusiveGroup( "Tools" );
    connect( m_selectTool, SIGNAL( toggled( bool ) ), this, SLOT( slotSelectTool( bool ) ) );
    m_pointTool = new KToggleAction( i18n("Point Tool"), "pointtool", 0, actionCollection(), "pointTool" );
    m_pointTool->setExclusiveGroup( "Tools" );
    connect( m_pointTool, SIGNAL( toggled( bool ) ), this, SLOT( slotPointTool( bool ) ) );
    m_freehandTool = new KToggleAction( i18n("Freehand Tool"), "freehandtool", 0, actionCollection(), "freehandTool" );
    m_freehandTool->setExclusiveGroup( "Tools" );
    connect( m_freehandTool, SIGNAL( toggled( bool ) ), this, SLOT( slotFreehandTool( bool ) ) );
    m_lineTool = new KToggleAction( i18n("Line Tool"), "linetool", 0, actionCollection(), "lineTool" );
    m_lineTool->setExclusiveGroup( "Tools" );
    connect( m_lineTool, SIGNAL( toggled( bool ) ), this, SLOT( slotLineTool( bool ) ) );
    m_bezierTool = new KToggleAction( i18n("Bezier Tool"), "beziertool", 0, actionCollection(), "bezierTool" );
    m_bezierTool->setExclusiveGroup( "Tools" );
    connect( m_bezierTool, SIGNAL( toggled( bool ) ), this, SLOT( slotBezierTool( bool ) ) );
    m_rectTool = new KToggleAction( i18n("Rect Tool"), "recttool", 0, actionCollection(), "rectTool" );
    m_rectTool->setExclusiveGroup( "Tools" );
    connect( m_rectTool, SIGNAL( toggled( bool ) ), this, SLOT( slotRectTool( bool ) ) );
    m_polygonTool = new KToggleAction( i18n("Polygon Tool"), "polygontool", 0, actionCollection(), "polygonTool" );
    m_polygonTool->setExclusiveGroup( "Tools" );
    connect( m_polygonTool, SIGNAL( toggled( bool ) ), this, SLOT( slotPolygonTool( bool ) ) );
    m_ellipseTool = new KToggleAction( i18n("Ellipse Tool"), "ellipsetool", 0, actionCollection(), "ellipseTool" );
    m_ellipseTool->setExclusiveGroup( "Tools" );
    connect( m_ellipseTool, SIGNAL( toggled( bool ) ), this, SLOT( slotEllipseTool( bool ) ) );
    m_textTool = new KToggleAction( i18n("Text Tool"), "texttool", 0, actionCollection(), "textTool" );
    m_textTool->setExclusiveGroup( "Tools" );
    connect( m_textTool, SIGNAL( toggled( bool ) ), this, SLOT( slotTextTool( bool ) ) );
    m_zoomTool = new KToggleAction( i18n("Zoom Tool"), "zoomtool", 0, actionCollection(), "zoomTool" );
    m_zoomTool->setExclusiveGroup( "Tools" );
    connect( m_zoomTool, SIGNAL( toggled( bool ) ), this, SLOT( slotZoomTool( bool ) ) );

    // Node Toolbar
    m_moveNode = new KToggleAction( i18n("Move Node "), "moveNode", 0, actionCollection(), "moveNode" );
    m_moveNode->setExclusiveGroup( "Node" );
    connect( m_outline, SIGNAL( toggled( bool ) ), this, SLOT( slotMoveNode( bool ) ) );
    m_newNode = new KToggleAction( i18n("New Node "), "newNode", 0, actionCollection(), "newNode" );
    m_newNode->setExclusiveGroup( "Node" );
    connect( m_outline, SIGNAL( toggled( bool ) ), this, SLOT( slotNewNode( bool ) ) );
    m_deleteNode = new KToggleAction( i18n("Delete Node "), "deleteNode", 0, actionCollection(), "deleteNode" );
    m_deleteNode->setExclusiveGroup( "Node" );
    connect( m_outline, SIGNAL( toggled( bool ) ), this, SLOT( slotDeleteNode( bool ) ) );
    m_splitLine = new KToggleAction( i18n("Move Node "), "split", 0, actionCollection(), "splitLine" );
    m_splitLine->setExclusiveGroup( "Node" );
    connect( m_outline, SIGNAL( toggled( bool ) ), this, SLOT( slotSplitLine( bool ) ) );

    m_selectTool->setChecked( TRUE );
    m_normal->setChecked( TRUE );
    m_showRuler->setChecked( TRUE );
    m_showHelplines->setChecked( TRUE );
    m_showGrid->setChecked( TRUE );

    // Disable node actions
    slotPointTool( FALSE );

    setupPopups ();
    setUndoStatus (false, false);
    QObject::connect (&cmdHistory, SIGNAL(changed(bool, bool)),
                      SLOT(setUndoStatus(bool, bool)));
}

void KIllustratorView::setupPopups()
{
    objMenu = new QPopupMenu ();
    m_copy->plug( objMenu );
    m_cut->plug( objMenu );
    objMenu->insertSeparator ();
    m_properties->plug( objMenu );
    objMenu->insertSeparator ();
    m_distribute->plug( objMenu );
    objMenu->insertSeparator ();
    m_distribute->plug( objMenu );
    m_toFront->plug( objMenu );
    m_toBack->plug( objMenu );
    m_forwardOne->plug( objMenu );
    m_backOne->plug( objMenu );
}

void KIllustratorView::setupCanvas()
{
    MeasurementUnit mu = PStateManager::instance ()->defaultMeasurementUnit ();
    hRuler = new Ruler (Ruler::Horizontal, mu, this);
    hRuler->setGeometry(30, 0, width()-30, 30);
    vRuler = new Ruler (Ruler::Vertical, mu, this);
    vRuler->setGeometry(0, 30, 30, height()-30);

    scrollview = new QScrollView(this);
    scrollview->setGeometry(30, 30, width()-30, height()-30);

    canvas = new Canvas (m_pDoc->gdoc(), 72.0, scrollview, scrollview->viewport());
    scrollview->addChild(canvas);
    //scrollview->viewport()->setBackgroundMode(QWidget::PaletteBackground);
    QObject::connect (canvas, SIGNAL(sizeChanged ()),
                      scrollview, SLOT(updateScrollBars()));
    QObject::connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
                      hRuler, SLOT(updateVisibleArea (int, int)));
    QObject::connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
                      vRuler, SLOT(updateVisibleArea (int, int)));

    QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
                      hRuler, SLOT(setZoomFactor (float)));
    QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
                      vRuler, SLOT(setZoomFactor (float)));
    //  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
    //       this, SLOT(updateZoomFactor (float)));
    //  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
    //       this, SLOT(showCursorPosition(int, int)));
    QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
                      hRuler, SLOT(updatePointer(int, int)));
    QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
                      vRuler, SLOT(updatePointer(int, int)));
    QObject::connect (canvas, SIGNAL(rightButtonAtSelectionClicked (int, int)),
                      this, SLOT(popupForSelection (int, int)));

    // helpline creation
    connect (hRuler, SIGNAL (drawHelpline(int, int, bool)),
             canvas, SLOT(drawTmpHelpline(int, int, bool)));
    connect (vRuler, SIGNAL (drawHelpline(int, int, bool)),
             canvas, SLOT(drawTmpHelpline(int, int, bool)));
    connect (hRuler, SIGNAL (addHelpline(int, int, bool)),
             canvas, SLOT(addHelpline(int, int, bool)));
    connect (vRuler, SIGNAL (addHelpline(int, int, bool)),
             canvas, SLOT(addHelpline(int, int, bool)));

    setFocusPolicy(QWidget::StrongFocus);
    setFocusProxy(canvas);

    tcontroller = new ToolController (this);

    SelectionTool* selTool;
    tcontroller->registerTool (ID_TOOL_SELECT,
                               selTool = new SelectionTool (&cmdHistory));
    QObject::connect (selTool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    //  QObject::connect (selTool, SIGNAL(partSelected(GObject*)),
    //                this, SLOT(activatePart(GObject*)));
    tcontroller->registerTool (ID_TOOL_EDITPOINT,
                               editPointTool = new EditPointTool (&cmdHistory));
    QObject::connect (editPointTool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    Tool* tool;
    tcontroller->registerTool (ID_TOOL_FREEHAND,
                               tool = new FreeHandTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_LINE,
                               tool = new PolylineTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_BEZIER,
                               tool = new BezierTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_RECTANGLE,
                               tool = new RectangleTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_POLYGON,
                               tool = new PolygonTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_ELLIPSE,
                               tool = new OvalTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_TEXT,
                               tool = new TextTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));
    tcontroller->registerTool (ID_TOOL_ZOOM,
                               tool = new ZoomTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(modeSelected(const QString&)),
                      this, SLOT(showCurrentMode(const QString&)));

    tcontroller->registerTool (ID_TOOL_PATHTEXT,
                               tool = new PathTextTool (&cmdHistory));
    QObject::connect (tool, SIGNAL(operationDone ()),
                      this, SLOT (resetTools ()));

    tcontroller->registerTool (ID_TOOL_INSERTPART,
                               insertPartTool =
                               new InsertPartTool (&cmdHistory));
    QObject::connect (insertPartTool, SIGNAL(operationDone ()),
                      this, SLOT (resetTools ()));

    tcontroller->toolSelected( ID_TOOL_SELECT );
    // m_idActiveTool = ID_TOOL_SELECT;

    canvas->setToolController(tcontroller);
}

void KIllustratorView::showCurrentMode (const QString& ) {
    //  statusbar->changeItem (msg, 2);
}

void KIllustratorView::setUndoStatus(bool undoPossible, bool redoPossible)
{
    m_undo->setEnabled( undoPossible );
    m_redo->setEnabled( redoPossible );

    QString label = i18n ("Undo");
    if (undoPossible)
        label += " " + cmdHistory.getUndoName ();
    m_undo->setText( label );

    label = i18n ("Redo");
    if (redoPossible)
        label += " " + cmdHistory.getRedoName ();
    m_redo->setText( label );
}

void KIllustratorView::resizeEvent(QResizeEvent* ) {
    if(m_bShowRulers) {
        hRuler->setGeometry(30, 0, width()-30, 30);
        vRuler->setGeometry(0, 30, 30, height()-30);
        scrollview->setGeometry(30, 30, width()-30, height()-30);
    }
    else
        scrollview->setGeometry(0, 0, width(), height());
}

void KIllustratorView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KIllustratorView::showTransformationDialog( int id )
{
    TransformationDialog *transformationDialog = new TransformationDialog (&cmdHistory);
    QObject::connect (m_pDoc, SIGNAL (selectionChanged ()),
                      transformationDialog, SLOT (update ()));
    transformationDialog->setDocument ( m_pDoc->gdoc() );
    transformationDialog->showTab (id);
}


bool KIllustratorView::printDlg()
{
    canvas->printDocument ();
    return true;
}

/*
void KIllustratorView::editInsertObject ()
{
    m_pDoc->gdoc()->unselectAllObjects();
    KoDocumentEntry docEntry = KoPartSelectDia::selectPart ();
    if (docEntry.name.isEmpty ())
        return;

    insertPartTool->setPartEntry (docEntry);
    // ####### Torben
    // tcontroller->toolSelected (m_idActiveTool = ID_TOOL_INSERTPART);
}
*/

/*
void KIllustratorView::setPenColor (long int id) {
  short int red, green, blue;
  bool fill;
  // ####### Torben
  // if(! m_vColorBar->getRgbColor (id, red, green, blue, fill))
  // return;

  GObject::OutlineInfo oInfo;
  oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;
  oInfo.color = QColor (red, green, blue);
  oInfo.style = fill ? SolidLine : NoPen;

  GObject::FillInfo fInfo;
  fInfo.mask = 0;

  if (! m_pDoc->gdoc()->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result =
      QMessageBox::warning (this, i18n("Warning"),
                            i18n ("This action will set the default\n"
                                  "properties for new objects !\n"
                                  "Would you like to do it ?"),
                            i18n ("Yes"), i18n ("No"));
    if (result == 0)
      GObject::setDefaultOutlineInfo (oInfo);
  }
}
*/
/*
void KIllustratorView::setFillColor (long int id) {
  short int red, green, blue;
  bool fill;
  // ###### Torben
  // if(! m_vColorBar->getRgbColor (id, red, green, blue, fill))
  // return;

  GObject::OutlineInfo oInfo;
  oInfo.mask = 0;

  GObject::FillInfo fInfo;
  fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  fInfo.color = QColor (red, green, blue);
  fInfo.fstyle = fill ? GObject::FillInfo::SolidFill :
    GObject::FillInfo::NoFill;

  if (! m_pDoc->gdoc()->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result =
      QMessageBox::warning(this, i18n("Warning"),
                           i18n ("This action will set the default\n"
                                 "properties for new objects !\n"
                                 "Would you like to do it ?"),
                           i18n ("Yes"), i18n ("No"));
    if (result == 0)
      GObject::setDefaultFillInfo (fInfo);
  }
}
*/

/*
void KIllustratorView::configPolygonTool () {
    // ###### Torben
    // tcontroller->configureTool (ID_TOOL_POLYGON);
}

void KIllustratorView::configEllipseTool () {
    // ###### Torben
    // tcontroller->configureTool (ID_TOOL_ELLIPSE);
}
*/

/*
void KIllustratorView::zoomSizeSelected (const QString & s)
{
  float value = s.toFloat();
  if (canvas)
    canvas->setZoomFactor (value / 100.0);
}
*/

void KIllustratorView::popupForSelection (int, int )
{
    objMenu->popup( QCursor::pos () );
}


void KIllustratorView::resetTools()
{
    m_selectTool->setEnabled( TRUE );
}

// void KIllustratorView::activatePart (GObject *obj) {
/* if (obj->isA ("GPart")) {
   GPart *part = (GPart *) obj;
   cout << "setFramesToParts ..." << endl;
   setFramesToParts ();
   cout << "part->activate ..." << endl;
   int xoff = 1, yoff = 1;
   if (m_bShowRulers) {
   xoff += 30;
   yoff += 30;
   }

   part->activate (xoff, yoff);
   setFocusProxy (part->getView ());
   QWidget::setFocusPolicy (QWidget::StrongFocus);
   cout << "setFocus ..." << endl;
   part->getView ()->setFocusPolicy (QWidget::StrongFocus);
   part->getView ()->setFocus ();
   } */
// }


GDocument* KIllustratorView::activeDocument()
{
    return m_pDoc->gdoc();
}

/*
void KIllustratorView::insertPartSlot( KIllustratorChild *, GPart *)
{
}

void KIllustratorView::changeChildGeometrySlot(KIllustratorChild *)
{
}
*/

QString KIllustratorView::getExportFileName (FilterManager *filterMgr)
{
    const char *defaultExt = 0L;
    QString extension;

    if (! lastExport.isEmpty ()) {
        int pos = lastExport.findRev ('.', -1, false);
        if (pos != -1) {
            extension =
                lastExport.right (lastExport.length () - pos - 1);
            defaultExt = (const char *) extension;
        }
    }
    QString filter = filterMgr->exportFilters (defaultExt);

#ifdef USE_QFD
    QString filename = QFileDialog::getSaveFileName( QString::null, filter, this );
#else
    KFileDialog *dlg = new KFileDialog (lastExportDir,
                                        filter, this,
                                        "file dia", true);
    dlg->setCaption (i18n ("Save As"));
    if (! lastExport.isEmpty ()) {
        dlg->setSelection (lastExport);
    }
    QString filename;

    if (dlg->exec() == QDialog::Accepted) {
        KURL url = dlg->selectedURL ();
        if (!url.isLocalFile())
            KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
        filename = url.path();
        lastExportDir = url.directory();
    }

    delete dlg;
#endif

    return filename;
}

// ---------------------------------------- actions

void KIllustratorView::slotImport()
{
    FilterManager* filterMgr = FilterManager::instance ();
    QString filter = filterMgr->importFilters ();

#ifdef USE_QFD
    QString fname = QFileDialog::getOpenFileName (lastImportDir, filter, this);
#else
    KURL url = KFileDialog::getOpenURL( lastImportDir, filter, this );
    if (!url.isLocalFile())
        KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
    QString fname = url.path();
#endif
    if (! fname.isEmpty ())
    {
        QFileInfo finfo ((const char *) fname);
        if (!finfo.isFile () || !finfo.isReadable ())
            return;

        lastImportDir = finfo.dirPath ();
        FilterInfo* filterInfo = filterMgr->findFilter (fname,
                                                        FilterInfo::FKind_Import);
        if (filterInfo)
        {
            ImportFilter* filter = filterInfo->importFilter ();
            if (filter->setup (m_pDoc->gdoc(), filterInfo->extension ()))
            {
                filter->setInputFileName (fname);
                filter->importFromFile (m_pDoc->gdoc());
            }
            else
                QMessageBox::critical (this, i18n ("KIllustrator Error"),
                                       i18n ("Cannot import from file"), i18n ("OK"));
        }
        else
            QMessageBox::critical (this, i18n ("KIllustrator Error"),
                                   i18n ("Unknown import format"), i18n ("OK"));
    }

    resetTools ();
}

void KIllustratorView::slotExport()
{
    FilterManager* filterMgr = FilterManager::instance ();
    QString filter = filterMgr->exportFilters ();

    QString fname = getExportFileName (filterMgr);

    if (! fname.isEmpty ())
    {
        FilterInfo* filterInfo = filterMgr->findFilter (fname,
                                                        FilterInfo::FKind_Export);

        if (filterInfo)
        {
            ExportFilter* filter = filterInfo->exportFilter ();
            if (filter->setup (m_pDoc->gdoc(), filterInfo->extension ()))
            {
                filter->setOutputFileName (fname);
                filter->exportToFile (m_pDoc->gdoc());
                lastExport = fname;
            }
            else
                QMessageBox::critical (this, i18n ("KIllustrator Error"),
                                       i18n ("Cannot export to file"), i18n ("OK"));
        }
        else
            QMessageBox::critical (this, i18n ("KIllustrator Error"),
                                   i18n ("Unknown export format"), i18n ("OK"));
    }
    resetTools ();
}

void KIllustratorView::slotInsertBitmap()
{
#ifdef USE_QFD
    QString fname = QFileDialog::getOpenFileName
                    ((const char *) lastBitmapDir, i18n("*.gif *.GIF | GIF Images\n"
                                                        "*.jpg *.jpeg *.JPG *.JPEG | JPEG Images\n"
                                                        "*.png | PNG Images\n"
                                                        "*.xbm | X11 Bitmaps\n"
                                                        "*.xpm | X11 Pixmaps"),
                     this);
#else
    KURL url = KFileDialog::getOpenURL
               (lastBitmapDir, i18n("*.gif *.GIF | GIF Images\n"
                                    "*.jpg *.jpeg *.JPG *.JPEG | JPEG Images\n"
                                    "*.png | PNG Images\n"
                                    "*.xbm | X11 Bitmaps\n"
                                    "*.xpm | X11 Pixmaps"),
                this);
    if (!url.isLocalFile())
        KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
    QString fname = url.path();
#endif
    if (! fname.isEmpty ()) {
        QFileInfo finfo (fname);
        lastBitmapDir = finfo.dirPath ();
        InsertPixmapCmd *cmd = new InsertPixmapCmd (m_pDoc->gdoc(),
                                                    (const char *) fname);
        cmdHistory.addCommand (cmd, true);
    }
}

void KIllustratorView::slotInsertClipart()
{
#ifdef USE_QFD
    QString fname = QFileDialog::getOpenFileName
                    (lastClipartDir,
                     i18n("*.wmf *.WMF | Windows Metafiles"), this);
#else
    KURL url = KFileDialog::getOpenURL( lastClipartDir,
                                        i18n("*.wmf *.WMF | Windows Metafiles"), this);
    if (!url.isLocalFile())
        KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
    QString fname = url.path();
#endif
    if ( !fname.isEmpty ())
    {
        QFileInfo finfo (fname);
        lastClipartDir = finfo.dirPath ();
        InsertClipartCmd *cmd = new InsertClipartCmd (m_pDoc->gdoc(),
                                                      (const char *) fname);
        cmdHistory.addCommand (cmd, true);
    }
}

void KIllustratorView::slotCopy()
{
    cmdHistory.addCommand (new CopyCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotPaste()
{
    cmdHistory.addCommand (new PasteCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotCut()
{
    cmdHistory.addCommand (new CutCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotUndo()
{
    cmdHistory.undo ();
    resetTools ();
}

void KIllustratorView::slotRedo()
{
    cmdHistory.redo ();
    resetTools ();
}

void KIllustratorView::slotDuplicate()
{
    cmdHistory.addCommand (new DuplicateCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotDelete()
{
    cmdHistory.addCommand (new DeleteCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotSelectAll()
{
    m_pDoc->gdoc()->selectAllObjects ();
}

void KIllustratorView::slotProperties()
{
    int result = 0;

    if (m_pDoc->gdoc()->selectionIsEmpty ())
    {
        result = QMessageBox::warning (this, i18n("Warning"),
                                       i18n ("This action will set the default\n"
                                             "properties for new objects !\n"
                                             "Would you like to do it ?"),
                                       i18n ("Yes"), i18n ("No"));
    }
    if (result == 0)
        PropertyEditor::edit( &cmdHistory, m_pDoc->gdoc() );
}

void KIllustratorView::slotOutline( bool )
{
    canvas->setOutlineMode (true);
}

void KIllustratorView::slotNormal( bool )
{
    canvas->setOutlineMode (false);
}

void KIllustratorView::slotShowRuler( bool b )
{
    m_bShowRulers = b;

    if (m_bShowRulers)
    {
        hRuler->show ();
        vRuler->show ();
    }
    else
    {
        hRuler->hide ();
        vRuler->hide ();
    }
    // recalculate layout
    resizeEvent (0L);
}

void KIllustratorView::slotShowGrid( bool b )
{
    canvas->showGrid( b );
}

void KIllustratorView::slotShowHelplines( bool b )
{
    canvas->showHelplines( b );
}

void KIllustratorView::slotPage()
{
    KoPageLayout pLayout = m_pDoc->gdoc()->pageLayout ();
    KoHeadFoot header;

    if (KoPageLayoutDia::pageLayout (pLayout, header, FORMAT_AND_BORDERS))
        m_pDoc->gdoc()->setPageLayout (pLayout);
}

void KIllustratorView::slotGrid()
{
    GridDialog::setupGrid (canvas);
}

void KIllustratorView::slotHelplines()
{
    HelplineDialog::setup (canvas);
}

void KIllustratorView::slotAlignToGrid( bool b )
{
    canvas->snapToGrid( b );
}

void KIllustratorView::slotAlignToHelplines( bool b )
{
    canvas->alignToHelplines( b );
}

void KIllustratorView::slotTransformPosition()
{
    showTransformationDialog( 0 );
}

void KIllustratorView::slotTransformDimension()
{
    showTransformationDialog( 1 );
}

void KIllustratorView::slotTransformRotation()
{
    showTransformationDialog( 2 );
}

void KIllustratorView::slotTransformMirror()
{
    showTransformationDialog( 3 );
}

void KIllustratorView::slotDistribute()
{
    AlignmentDialog::alignSelection (m_pDoc->gdoc(), &cmdHistory);
}

void KIllustratorView::slotToFront()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToFront), true);
}

void KIllustratorView::slotToBack()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToBack), true);
}

void KIllustratorView::slotForwardOne()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ForwardOne), true);
}

void KIllustratorView::slotBackOne()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_BackwardOne), true);
}

void KIllustratorView::slotGroup()
{
    cmdHistory.addCommand (new GroupCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotUngroup()
{
    cmdHistory.addCommand (new UngroupCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotTextAlongPath()
{
    tcontroller->toolSelected (ID_TOOL_PATHTEXT);
}

void KIllustratorView::slotConvertToCurve()
{
    if ( !m_pDoc->gdoc()->selectionIsEmpty() )
        cmdHistory.addCommand (new ToCurveCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotBlend()
{
    if ( m_pDoc->gdoc()->selectionCount () == 2)
    {
        int steps = BlendDialog::getNumOfSteps ();
        if (steps > 0)
            cmdHistory.addCommand (new BlendCmd (m_pDoc->gdoc(), steps), true);
    }
}

void KIllustratorView::slotOptions()
{
    OptionDialog::setup ();
}

void KIllustratorView::slotBrushChosen( const QColor & c )
{
    // #### Torben: ..... hmmmmm
    bool fill = TRUE;

    GObject::OutlineInfo oInfo;
    oInfo.mask = 0;

    GObject::FillInfo fInfo;
    fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
    fInfo.color = c;
    fInfo.fstyle = fill ? GObject::FillInfo::SolidFill :
                GObject::FillInfo::NoFill;

    if ( !m_pDoc->gdoc()->selectionIsEmpty () )
    {
        SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
        cmdHistory.addCommand (cmd, true);
    }
    else
    {
        int result = QMessageBox::warning(this, i18n("Warning"),
                                          i18n ("This action will set the default\n"
                                                "properties for new objects !\n"
                                                "Would you like to do it ?"),
                                          i18n ("Yes"), i18n ("No"));
        if (result == 0)
            GObject::setDefaultFillInfo (fInfo);
    }
}

void KIllustratorView::slotPenChosen( const QColor & c  )
{
    // #### Torben: ..... hmmmmm
    bool fill = TRUE;

    GObject::OutlineInfo oInfo;
    oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;
    oInfo.color = c                        ;
    oInfo.style = fill ? SolidLine : NoPen;

    GObject::FillInfo fInfo;
    fInfo.mask = 0;

    if (! m_pDoc->gdoc()->selectionIsEmpty () )
    {
        SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
        cmdHistory.addCommand (cmd, true);
    }
    else
    {
        int result = QMessageBox::warning (this, i18n("Warning"),
                                           i18n ("This action will set the default\n"
                                                 "properties for new objects !\n"
                                                 "Would you like to do it ?"),
                                           i18n ("Yes"), i18n ("No"));
        if (result == 0)
            GObject::setDefaultOutlineInfo (oInfo);
    }
}

void KIllustratorView::slotSelectTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_SELECT );
}

void KIllustratorView::slotPointTool( bool b )
{
    m_moveNode->setEnabled( b );
    m_newNode->setEnabled( b );
    m_deleteNode->setEnabled( b );
    m_splitLine->setEnabled( b );

    if ( b )
        slotMoveNode( TRUE );

    tcontroller->toolSelected( ID_TOOL_EDITPOINT );
}

void KIllustratorView::slotFreehandTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_FREEHAND );
}

void KIllustratorView::slotLineTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_LINE);
}

void KIllustratorView::slotBezierTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_BEZIER);
}

void KIllustratorView::slotRectTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_RECTANGLE );
}

void KIllustratorView::slotPolygonTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_POLYGON );
}

void KIllustratorView::slotEllipseTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_ELLIPSE );
}

void KIllustratorView::slotTextTool( bool b )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_TEXT );
}

void KIllustratorView::slotZoomTool( bool b  )
{
    if ( b )
        tcontroller->toolSelected( ID_TOOL_ZOOM );
}

void KIllustratorView::slotMoveNode( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::MovePoint);
}

void KIllustratorView::slotNewNode( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::InsertPoint);
}

void KIllustratorView::slotDeleteNode( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::RemovePoint);
}

void KIllustratorView::slotSplitLine( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::Split);
}

void KIllustratorView::slotLayers()
{
    if (!layerDialog)
        layerDialog = new LayerDialog ();
    layerDialog->manageDocument (m_pDoc->gdoc());
    layerDialog->show ();
}

void KIllustratorView::slotLoadPalette () {
    // TODO
}

void KIllustratorView::slotViewZoom (const QString& s) {
    QString z (s);
    z = z.replace (QRegExp ("%"), "");
    z = z.simplifyWhiteSpace ();
    float zoom = z.toFloat () / 100.0;
    if (zoom != canvas->getZoomFactor ())
        canvas->setZoomFactor (zoom);
}

#include "KIllustrator_view.moc"
