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
#include "MainView.h"

#include "QwViewport.h"
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
#include "ScriptDialog.h"
#include "PStateManager.h"
#include "ExportFilter.h"
#include "AboutDialog.h"
#include "InsertClipartCmd.h"
#include "GroupCmd.h"
#include "UngroupCmd.h"
#include "DeleteCmd.h"
#include "CutCmd.h"
#include "CopyCmd.h"
#include "PasteCmd.h"
#include "DuplicateCmd.h"
#include "ReorderCmd.h"
#include "SetPropertyCmd.h"
#include "InsertClipartCmd.h"
#include "InsertPixmapCmd.h"
#include "SetPropertyCmd.h"
// #include "FilterManager.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <unistd.h>

#include <koPartSelectDia.h>

KIllustratorView::KIllustratorView (QWidget* parent, const char* name,
				    KIllustratorDocument* doc) :
    ContainerView( doc, parent, name )
{
  m_pDoc = doc;
  m_bShowGUI = true;
  m_bShowRulers = true;
  mainWidget = 0L;
  viewport = 0L;
  layerDialog = 0L;
  scriptDialog = 0L;
  objMenu = 0L;
  // restore default settings
  PStateManager::instance ();

  zFactors.resize (8);
  zFactors[0] = 0.5;
  zFactors[1] = 1.0;
  zFactors[2] = 1.5;
  zFactors[3] = 2.0;
  zFactors[4] = 4.0;
  zFactors[5] = 6.0;
  zFactors[6] = 8.0;
  zFactors[7] = 10.0;

  Canvas::initZoomFactors (zFactors);

  cout << "connect doc" << endl;
  QObject::connect (m_pDoc,
		    SIGNAL (partInserted (KIllustratorChild *, GPart *)),
		    this,
		    SLOT (insertPartSlot (KIllustratorChild *, GPart *)));
  QObject::connect (m_pDoc,
		    SIGNAL (childGeometryChanged (KIllustratorChild *)),
		   this, SLOT(changeChildGeometrySlot (KIllustratorChild *)));
  createGUI ();
}

KIllustratorView::~KIllustratorView()
{
  cout << "~KIllustratorView ()" << endl;
}

void KIllustratorView::createGUI()
{
  setupCanvas ();
  setupPopups ();
  setUndoStatus (false, false);
  QObject::connect (&cmdHistory, SIGNAL(changed(bool, bool)),
		    SLOT(setUndoStatus(bool, bool)));
}

void KIllustratorView::setupPopups()
{
  objMenu = new QPopupMenu ();
  objMenu->insertItem (i18n ("Copy"), this, SLOT (editCopySlot ()));
  objMenu->insertItem (i18n ("Cut"), this, SLOT (editCutSlot ()));
  objMenu->insertSeparator ();
  objMenu->insertItem (i18n ("Properties..."), this,
		       SLOT (editPropertiesSlot ()));
  objMenu->insertSeparator ();
  objMenu->insertItem (i18n ("Align"), this, SLOT (arrangeAlignSlot ()));
  objMenu->insertSeparator ();
  objMenu->insertItem (i18n ("To Front"), this, SLOT (arrangeToFrontSlot ()));
  objMenu->insertItem (i18n ("To Back"), this, SLOT (arrangeToBackSlot ()));
  objMenu->insertItem (i18n ("Forward One"), this,
		       SLOT (arrangeOneForwardSlot ()));
  objMenu->insertItem (i18n ("Back One"), this, SLOT (arrangeOneBackSlot ()));
}

void KIllustratorView::setupCanvas()
{
  QWidget *w = new QWidget (this);
  w->resize (500, 500);

  grid = new QGridLayout (w, 2, 2);

  MeasurementUnit mu = PStateManager::instance ()->defaultMeasurementUnit ();
  hRuler = new Ruler (Ruler::Horizontal, mu, w);
  vRuler = new Ruler (Ruler::Vertical, mu, w);
  grid->addWidget (hRuler, 0, 1);
  grid->addWidget (vRuler, 1, 0);

  viewport = new QwViewport (w);

  canvas = new Canvas (m_pDoc->gdoc(), 72.0, viewport, viewport->portHole ());
  QObject::connect (canvas, SIGNAL(sizeChanged ()),
           viewport, SLOT(resizeScrollBars ()));
  QObject::connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
	   hRuler, SLOT(updateVisibleArea (int, int)));
  QObject::connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
	   vRuler, SLOT(updateVisibleArea (int, int)));

  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   hRuler, SLOT(setZoomFactor (float)));
  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   vRuler, SLOT(setZoomFactor (float)));
  //  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
  //	   this, SLOT(updateZoomFactor (float)));
  //  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
  //	   this, SLOT(showCursorPosition(int, int)));
  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   hRuler, SLOT(updatePointer(int, int)));
  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   vRuler, SLOT(updatePointer(int, int)));
  QObject::connect (canvas, SIGNAL(rightButtonAtSelectionClicked (int, int)),
	   this, SLOT(popupForSelection (int, int)));

  setFocusPolicy (QWidget::StrongFocus);
  setFocusProxy (canvas);

  grid->addWidget (viewport, 1, 1);
  grid->setRowStretch (1, 20);
  grid->setColStretch (1, 20);

  tcontroller = new ToolController (this);

  SelectionTool* selTool;
  tcontroller->registerTool (ID_TOOL_SELECT,
			     selTool = new SelectionTool (&cmdHistory));
  QObject::connect (selTool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  QObject::connect (selTool, SIGNAL(partSelected(GObject*)),
		    this, SLOT(activatePart(GObject*)));
  tcontroller->registerTool (ID_TOOL_EDITPOINT,
			     editPointTool = new EditPointTool (&cmdHistory));
  QObject::connect (editPointTool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  Tool* tool;
  tcontroller->registerTool (ID_TOOL_FREEHAND,
			     tool = new FreeHandTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_LINE,
			     tool = new PolylineTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_BEZIER,
			     tool = new BezierTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_RECTANGLE,
			     tool = new RectangleTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_POLYGON,
			     tool = new PolygonTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_ELLIPSE,
			     tool = new OvalTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_TEXT,
			     tool = new TextTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (ID_TOOL_ZOOM,
			     tool = new ZoomTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));

  tcontroller->registerTool (ID_TOOL_PATHTEXT,
			     tool = new PathTextTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(operationDone ()),
		    this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_INSERTPART,
			     insertPartTool =
			     new InsertPartTool (&cmdHistory));
  QObject::connect (insertPartTool, SIGNAL(operationDone ()),
		    this, SLOT (resetTools ()));

  tcontroller->toolSelected (ID_TOOL_SELECT);
  m_idActiveTool = ID_TOOL_SELECT;

  canvas->setToolController (tcontroller);
  grid->activate ();
  w->show ();
  mainWidget = w;
}

void KIllustratorView::showCurrentMode (const char* ) {
    //  statusbar->changeItem (msg, 2);
}


void KIllustratorView::newView ()
{
    m_pDoc->createShell();
}

void KIllustratorView::setUndoStatus(bool undoPossible, bool redoPossible)
{
    // ########## Torben
    /* if (! CORBA::is_nil (m_vMenuEdit)) {
    // we do this " " trick to avoid double translation of "Undo" and "Undo "
    m_vMenuEdit->setItemEnabled (m_idMenuEdit_Undo, undoPossible);

    QString text;

    QString label = i18n ("Undo");
    if (undoPossible)
      label += " " + cmdHistory.getUndoName ();
    text = label;
    m_vMenuEdit->changeItemText (text, m_idMenuEdit_Undo);

    m_vMenuEdit->setItemEnabled (m_idMenuEdit_Redo, redoPossible);

    label = i18n ("Redo");
    if (redoPossible)
      label += " " + cmdHistory.getRedoName ();

    text = label;
    m_vMenuEdit->changeItemText (text, m_idMenuEdit_Redo);
    } */
}

void KIllustratorView::resizeEvent (QResizeEvent* )
{
    // ######## Torben
  //  cout << "resizeEvent" << endl;
  if ( mainWidget )
  {
      mainWidget->resize( width(), height() );
      /* if ((KoViewIf::hasFocus () || mode () == KOffice::View::RootMode) &&
	 m_bShowGUI) */
      {
	  if (m_bShowRulers)
          {
	      // draw rulers
	      hRuler->show ();
	      vRuler->show ();
	  }
	  viewport->showScrollBars ();
      }
      /*      else
      {
	  hRuler->hide ();
	  vRuler->hide ();
	  viewport->hideScrollBars ();
	  grid->activate ();
	  } */
  }
}

void KIllustratorView::showTransformationDialog( int id )
{
    TransformationDialog *transformationDialog = new TransformationDialog (&cmdHistory);
    QObject::connect (m_pDoc, SIGNAL (selectionChanged ()),
		      transformationDialog, SLOT (update ()));
    transformationDialog->setDocument ( m_pDoc->gdoc() );
    transformationDialog->showTab (id);
}


bool KIllustratorView::printDlg () {
  canvas->printDocument ();
  return true;
}

void KIllustratorView::editCutSlot () {
  editCut ();
}

void KIllustratorView::editCopySlot () {
  editCopy ();
}

void KIllustratorView::editPropertiesSlot () {
  editProperties ();
}

void KIllustratorView::arrangeAlignSlot () {
  arrangeAlign ();
}

void KIllustratorView::arrangeToFrontSlot () {
  arrangeToFront ();
}

void KIllustratorView::arrangeToBackSlot () {
  arrangeToBack ();
}

void KIllustratorView::arrangeOneForwardSlot () {
  arrangeOneForward ();
}

void KIllustratorView::arrangeOneBackSlot () {
  arrangeOneBack ();
}

void KIllustratorView::editUndo () {
  cmdHistory.undo ();
  //  m_rToolBarTools->setButton (m_idActiveTool, false);
  //  tcontroller->toolSelected (m_idActiveTool = m_idSelectionTool);
  resetTools ();
}

void KIllustratorView::editRedo () {
  cmdHistory.redo ();
  //  m_rToolBarTools->setButton (m_idActiveTool, false);
  //  tcontroller->toolSelected (m_idActiveTool = m_idSelectionTool);
  resetTools ();
}

void KIllustratorView::editCut () {
  cmdHistory.addCommand (new CutCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::editCopy () {
  cmdHistory.addCommand (new CopyCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::editPaste () {
  cmdHistory.addCommand (new PasteCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::editSelectAll () {
  m_pDoc->gdoc()->selectAllObjects ();
}

void KIllustratorView::editDelete () {
  cmdHistory.addCommand (new DeleteCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::editDuplicate () {
  cmdHistory.addCommand (new DuplicateCmd (m_pDoc->gdoc()), true);
}

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

void KIllustratorView::editInsertClipart () {
  QString fname = KFilePreviewDialog::getOpenFileName
    (QString::null, "*.wmf *.WMF | Windows Metafiles", this);
  if (! fname.isEmpty ()) {
    InsertClipartCmd *cmd = new InsertClipartCmd (m_pDoc->gdoc(),
						  (const char *) fname);
    cmdHistory.addCommand (cmd, true);
  }
}

void KIllustratorView::editInsertBitmap () {
  QString fname = KFilePreviewDialog::getOpenFileName
    (QString::null, "*.gif *.GIF | GIF Images\n"
     "*.jpg *.jpeg *.JPG *.JPEG | JPEG Images\n"
     "*.png | PNG Images\n"
     "*.xbm | X11 Bitmaps\n"
     "*.xpm | X11 Pixmaps",
     this);
  if (! fname.isEmpty ()) {
    InsertPixmapCmd *cmd = new InsertPixmapCmd (m_pDoc->gdoc(),
						(const char *) fname);
    cmdHistory.addCommand (cmd, true);
  }
}

void KIllustratorView::editProperties () {
  int result = 1;

  if (m_pDoc->gdoc()->selectionIsEmpty ()) {
    result = QMessageBox::warning (this, i18n("Warning"),
				   i18n ("This action will set the default\n"
					 "properties for new objects !\n"
					 "Would you like to do it ?"),
				   i18n ("Yes"), i18n ("No"));
  }
  if (result == 0)
    PropertyEditor::edit (&cmdHistory, m_pDoc->gdoc());
}

void KIllustratorView::toggleRuler () {
  m_bShowRulers = !m_bShowRulers;
  // ##### Torben
  // m_vMenuView->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);
  if (m_bShowRulers) {
    hRuler->show ();
    vRuler->show ();
  }
  else {
    hRuler->hide ();
    vRuler->hide ();
  }
  // recalculate layout
  grid->activate ();
  resizeEvent (0L);
}

void KIllustratorView::arrangeAlign () {
  AlignmentDialog::alignSelection (m_pDoc->gdoc(), &cmdHistory);
}

void KIllustratorView::arrangeToFront () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToFront), true);
}

void KIllustratorView::arrangeToBack () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToBack), true);
}

void KIllustratorView::arrangeOneForward () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ForwardOne), true);
}

void KIllustratorView::arrangeOneBack () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_BackwardOne), true);
}

void KIllustratorView::arrangeGroup () {
  cmdHistory.addCommand (new GroupCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::arrangeUngroup () {
  cmdHistory.addCommand (new UngroupCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::arrangeTextAlongPath () {
    // ####### Torben
    // tcontroller->toolSelected (m_idActiveTool = ID_TOOL_PATHTEXT);
}

void KIllustratorView::transformPosition () {
  showTransformationDialog (0);
}

void KIllustratorView::transformDimension () {
  showTransformationDialog (1);
}

void KIllustratorView::transformRotation () {
  showTransformationDialog (2);
}

void KIllustratorView::transformMirror () {
  showTransformationDialog (3);
}

void KIllustratorView::toggleGrid () {
  bool gridIsShown = ! canvas->showGrid ();
  canvas->showGrid (gridIsShown);
  // ###### Torben
  // m_vMenuView->setItemChecked (m_idMenuView_Grid, gridIsShown);
}

void KIllustratorView::toggleHelplines () {
  bool linesAreShown = ! canvas->showHelplines ();
  canvas->showHelplines (linesAreShown);
  // ###### Torben
  // m_vMenuView->setItemChecked (m_idMenuView_Helplines, linesAreShown);
}

void KIllustratorView::setupGrid () {
  GridDialog::setupGrid (canvas);
}

void KIllustratorView::alignToGrid () {
  bool snap = ! canvas->snapToGrid ();
  canvas->snapToGrid (snap);
  // ###### Torben
  // m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToGrid, snap);
}

void KIllustratorView::setupHelplines () {
  HelplineDialog::setup (canvas);
}

void KIllustratorView::alignToHelplines () {
  bool snap = ! canvas->alignToHelplines ();
  canvas->alignToHelplines (snap);
  // ###### Torben
  // m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToHelplines, snap);
}

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

void KIllustratorView::editLayers () {
  if (!layerDialog)
    layerDialog = new LayerDialog ();
  layerDialog->manageDocument (m_pDoc->gdoc());
  layerDialog->show ();
}

void KIllustratorView::toolSelection () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_SELECT); */
}

void KIllustratorView::toolEditPoint () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  toolMovePoint ();
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_MOVE, true);
  m_vToolBarEditPoint->enable (OpenPartsUI::Show);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_EDITPOINT); */
}

void KIllustratorView::toolFreehandLine () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_FREEHAND); */
}

void KIllustratorView::toolPolyline () {
      // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_LINE); */
}

void KIllustratorView::toolBezier () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_BEZIER); */
}

void KIllustratorView::toolRectangle () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_RECTANGLE); */
}

void KIllustratorView::toolPolygon () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_POLYGON); */
}

void KIllustratorView::toolEllipse () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_ELLIPSE); */
}

void KIllustratorView::toolText () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_TEXT); */
}

void KIllustratorView::toolZoom () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_ZOOM); */
}

void KIllustratorView::configPolygonTool () {
    // ###### Torben
    // tcontroller->configureTool (ID_TOOL_POLYGON);
}

void KIllustratorView::configEllipseTool () {
    // ###### Torben
    // tcontroller->configureTool (ID_TOOL_ELLIPSE);
}

void KIllustratorView::viewOutline () {
  canvas->setOutlineMode (true);
  // ###### Torben
  // m_vMenuView->setItemChecked (m_idMenuView_Outline, true);
  // m_vMenuView->setItemChecked (m_idMenuView_Normal, false);
}

void KIllustratorView::viewNormal () {
  canvas->setOutlineMode (false);
  // ###### Torben
  // m_vMenuView->setItemChecked (m_idMenuView_Outline, false);
  // m_vMenuView->setItemChecked (m_idMenuView_Normal, true);
}

void KIllustratorView::setupPage () {
  KoPageLayout pLayout = m_pDoc->gdoc()->pageLayout ();
  KoHeadFoot header;

  if (KoPageLayoutDia::pageLayout (pLayout, header,
				   FORMAT_AND_BORDERS))
    m_pDoc->gdoc()->setPageLayout (pLayout);
}

void KIllustratorView::zoomSizeSelected (const QString & s)
{
  float value = s.toFloat();
  if (canvas)
    canvas->setZoomFactor (value / 100.0);
}

void KIllustratorView::popupForSelection (int, int )
{
  objMenu->popup (QCursor::pos ());
}

void KIllustratorView::toolMovePoint ()
{
  editPointTool->setMode (EditPointTool::MovePoint);
  // ###### Torben
  // m_vToolBarEditPoint->setButton (ID_TOOL_EP_INSERT, false);
  // m_vToolBarEditPoint->setButton (ID_TOOL_EP_DELETE, false);
}

void KIllustratorView::toolInsertPoint () {
  editPointTool->setMode (EditPointTool::InsertPoint);
  // ###### Torben
  // m_vToolBarEditPoint->setButton (ID_TOOL_EP_MOVE, false);
  // m_vToolBarEditPoint->setButton (ID_TOOL_EP_DELETE, false);
}

void KIllustratorView::toolRemovePoint () {
  editPointTool->setMode (EditPointTool::RemovePoint);
  // ###### Torben
  // m_vToolBarEditPoint->setButton (ID_TOOL_EP_MOVE, false);
  // m_vToolBarEditPoint->setButton (ID_TOOL_EP_INSERT, false);
}

void KIllustratorView::resetTools () {
    // ###### Torben
    /* m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_SELECT);
  m_vToolBarTools->setButton (m_idActiveTool, true); */
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

void KIllustratorView::showScripts () {
  if (!scriptDialog)
    scriptDialog = new ScriptDialog ();
  scriptDialog->setActiveDocument (m_pDoc->gdoc());
  scriptDialog->show ();
  scriptDialog->loadScripts ();
}

GDocument* KIllustratorView::activeDocument()
{
    return m_pDoc->gdoc();
}

void KIllustratorView::insertPartSlot( KIllustratorChild *, GPart *)
{
}

void KIllustratorView::changeChildGeometrySlot(KIllustratorChild *)
{
}

#include "KIllustrator_view.moc"
