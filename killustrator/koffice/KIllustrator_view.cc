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
#include "KIllustrator_view.moc"

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
#include "PropertyEditor.h"
#include "AlignmentDialog.h"
#include "GridDialog.h"
#include "TransformationDialog.h"
#include "LayerDialog.h"
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
#include "ReorderCmd.h"
#include "SetPropertyCmd.h"
#include "FilterManager.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kcombo.h>
#include <qlayout.h>
#include <unistd.h>

KIllustratorFrame::KIllustratorFrame (KIllustratorView* view, 
				      KIllustratorChild* child) :
  KoFrame (view) {
  m_pView = view;
  m_pChild = child;
}

KIllustratorView::KIllustratorView (QWidget* parent, const char* name, 
				    KIllustratorDocument* doc) :
QWidget (parent), KoViewIf (doc), OPViewIf (doc), KIllustrator::View_skel () {
  setWidget (this);
  OPPartIf::setFocusPolicy (OpenParts::Part::ClickFocus);

  m_lstFrames.setAutoDelete (true);
  m_pDoc = doc;
  m_bShowGUI = true;
  m_bShowRulers = true;
  mainWidget = 0L;
  viewport = 0L;
  layerDialog = 0L;
  // restore default settings
  PStateManager::instance ();

  const QColor cpalette[] = { white, red, green, blue, cyan, magenta, yellow,
			      darkRed, darkGreen, darkBlue, darkCyan,
			      darkMagenta, darkYellow, white, lightGray,
			      gray, darkGray, black };
  colorPalette.resize (18);
  for (int i = 0; i < 18; i++)
    colorPalette.insert (i, new QColor (cpalette[i]));

  zFactors.resize (5);
  zFactors[0] = 0.5;
  zFactors[1] = 1.0;
  zFactors[2] = 1.5;
  zFactors[3] = 2.0;
  zFactors[4] = 4.0;
  Canvas::initZoomFactors (zFactors);

  createGUI ();
}

void KIllustratorView::init () {
  cerr << "Registering menu as " << id () << endl;
  
  OpenParts::MenuBarManager_var menu_bar_manager = 
    m_vMainWindow->menuBarManager ();
  if (! CORBA::is_nil (menu_bar_manager))
    menu_bar_manager->registerClient (id (), this);
  else
    cerr << "Did not get a menu bar manager" << endl;

  /******************************************************
   * Toolbar
   ******************************************************/

  OpenParts::ToolBarManager_var tool_bar_manager = 
    m_vMainWindow->toolBarManager ();
  if (! CORBA::is_nil (tool_bar_manager))
    tool_bar_manager->registerClient (id (), this);
  else
    cerr << "Did not get a tool bar manager" << endl;  
}

KIllustratorView::~KIllustratorView () {
  cleanUp ();
}

void KIllustratorView::createGUI () {
  setupToolsToolbar ();
  setupColorToolbar ();
  setupCanvas ();
  setUndoStatus (false, false);
  QObject::connect (&cmdHistory, SIGNAL(changed(bool, bool)), 
		    SLOT(setUndoStatus(bool, bool)));
}

bool KIllustratorView::event (const char* _event, const CORBA::Any& _value) {
  EVENT_MAPPER (_event, _value);

  MAPPING (OpenPartsUI::eventCreateMenuBar, 
	   OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenubar);
  MAPPING (OpenPartsUI::eventCreateToolBar, 
	   OpenPartsUI::typeCreateToolBar_var, mappingCreateToolbar);

  END_EVENT_MAPPER;
  
  return false;
}

bool KIllustratorView::mappingCreateMenubar (OpenPartsUI::MenuBar_ptr 
					     menubar) {
  if (CORBA::is_nil (menubar)) {
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuLayout = 0L;
    m_vMenuArrange = 0L;
    m_vMenuTransform = 0L;
    m_vMenuExtras = 0L;
    m_vMenuHelp = 0L;
    return true;
  }

  // Menu: Edit  
  menubar->insertMenu (i18n ("&Edit"), m_vMenuEdit, -1, -1);
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem (i18n ("Undo"), this, 
					       "editUndo", 0);
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem (i18n ("Redo"), this, 
					       "editRedo", 0);
  m_vMenuEdit->insertSeparator (-1);
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem (i18n ("&Copy"), this, 
					   "editCopy", 0); 
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem (i18n ("&Paste"), this, 
					    "editPaste", 0);
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem (i18n ("C&ut"), this, 
					      "editCut", 0);
  m_vMenuEdit->insertSeparator (-1);
  m_idMenuEdit_Delete = m_vMenuEdit->insertItem (i18n ("&Delete"), this,
						 "editDelete", 0);
  m_vMenuEdit->insertSeparator (-1);
  m_idMenuEdit_SelectAll = m_vMenuEdit->insertItem (i18n ("&Select All"), this,
						    "editSelectAll", 0);
  m_vMenuEdit->insertSeparator (-1);
  m_idMenuEdit_InsertObject = 
    m_vMenuEdit->insertItem (i18n ("&Insert Object..."), this,
			     "editInsertObject", 0);
  m_idMenuEdit_Properties = 
    m_vMenuEdit->insertItem (i18n ("Pr&operties"), this,
			     "editProperties", 0);

  // Menu: View
  menubar->insertMenu (i18n ("&View"), m_vMenuView, -1, -1);
  m_idMenuView_Outline =
    m_vMenuView->insertItem (i18n ("Outline"), this, "viewOutline", 0);
  m_vMenuView->setCheckable (true);
  m_idMenuView_Normal =
    m_vMenuView->insertItem (i18n ("Normal"), this, "viewNormal", 0);
  m_vMenuView->insertSeparator (-1);
  m_idMenuView_Ruler =
    m_vMenuView->insertItem (i18n ("Ruler"), this, "toggleRuler", 0);
  m_vMenuView->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);
  m_idMenuView_Grid =
    m_vMenuView->insertItem (i18n ("Grid"), this, "toggleGrid", 0);
  m_vMenuView->setItemChecked (m_idMenuView_Grid, false);

  // Menu: Layout
  menubar->insertMenu (i18n ("&Layout"), m_vMenuLayout, -1, -1);
  m_idMenuLayout_PageLayout =
    m_vMenuLayout->insertItem (i18n ("Page Layout"), this, "setupPage", 0);
  m_vMenuLayout->insertSeparator (-1);
  m_idMenuLayout_Layers =
    m_vMenuLayout->insertItem (i18n ("Layers"), this, "editLayers", 0);
  m_vMenuLayout->insertSeparator (-1);
  m_idMenuLayout_SetupGrid =
    m_vMenuLayout->insertItem (i18n ("Setup Grid"), this, "setupGrid", 0);
  m_idMenuLayout_AlignToGrid =
    m_vMenuLayout->insertItem (i18n ("Align to Grid"), this, "alignToGrid", 0);
  m_vMenuLayout->setCheckable (true);
  m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToGrid, false);

  // Menu: Arrange
  menubar->insertMenu (i18n ("&Arrange"), m_vMenuArrange, -1, -1);
  m_vMenuArrange->insertItem8 (i18n ("Transform"), m_vMenuTransform, -1, -1);
  m_idMenuArrange_Align = m_vMenuArrange->insertItem (i18n ("Align"), this, 
						      "arrangeAlign", 0);
  m_idMenuArrange_ToFront = m_vMenuArrange->insertItem (i18n ("To Front"), 
							this, 
							"arrangeToFront", 0);
  m_idMenuArrange_ToBack = m_vMenuArrange->insertItem (i18n ("To Back"), this, 
				"arrangeToBack", 0);
  m_idMenuArrange_1Forward = 
    m_vMenuArrange->insertItem (i18n ("Forward One"), this, 
				"arrangeOneForward", 0);
  m_idMenuArrange_1Back = m_vMenuArrange->insertItem (i18n ("Back One"), this, 
						      "arrangeOneBack", 0);
  m_vMenuArrange->insertSeparator (-1);
  m_idMenuArrange_Group = m_vMenuArrange->insertItem (i18n ("Group"), this, 
						      "arrangeGroup", 0);
  m_idMenuArrange_Ungroup = m_vMenuArrange->insertItem (i18n ("Ungroup"), 
							this, 
							"arrangeUngroup", 0);

  // Menu: Arrange->Transform
  m_idMenuTransform_Position =
    m_vMenuTransform->insertItem (i18n ("Position"), this, 
				  "transformPosition", 0);
  m_idMenuTransform_Dimension =
    m_vMenuTransform->insertItem (i18n ("Dimension"), this, 
				  "transformDimension", 0);
  m_idMenuTransform_Rotation =
    m_vMenuTransform->insertItem (i18n ("Rotation"), this, 
				  "transformRotation", 0);
  m_idMenuTransform_Mirror =
    m_vMenuTransform->insertItem (i18n ("Mirror"), this, 
				  "transformMirror", 0);

  // Menu: Extras  
  menubar->insertMenu (i18n ("&Extras"), m_vMenuExtras, -1, -1);

  // Menu: Help
  m_vMenuHelp = menubar->helpMenu ();
  if (CORBA::is_nil (m_vMenuHelp)) {
    menubar->insertSeparator (-1);
    menubar->setHelpMenu (menubar->insertMenu (i18n ("&Help"),
					       m_vMenuHelp, -1, -1));
  }
  return true;
}

bool KIllustratorView::mappingCreateToolbar (OpenPartsUI::ToolBarFactory_ptr 
					     factory) {
  return true;
}

void KIllustratorView::setupCanvas () {
  QWidget *w = new QWidget (this);
  w->resize (500, 500);
  
  grid = new QGridLayout (w, 2, 2);
  
  MeasurementUnit mu = PStateManager::instance ()->defaultMeasurementUnit ();
  hRuler = new Ruler (Ruler::Horizontal, mu, w);
  vRuler = new Ruler (Ruler::Vertical, mu, w);
  grid->addWidget (hRuler, 0, 1);
  grid->addWidget (vRuler, 1, 0);

  viewport = new QwViewport (w);

  canvas = new Canvas (m_pDoc, 72.0, viewport, viewport->portHole ());
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

  grid->addWidget (viewport, 1, 1);
  grid->setRowStretch (1, 20);
  grid->setColStretch (1, 20);

  tcontroller = new ToolController (this);
  Tool* tool;
  tcontroller->registerTool (m_idSelectionTool, 
			     tool = new SelectionTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idEditPointTool, 
			     tool = new EditPointTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idFreeHandTool, 
			     tool = new FreeHandTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idPolylineTool, 
			     tool = new PolylineTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idBezierTool, 
			     tool = new BezierTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idRectangleTool, 
			     tool = new RectangleTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idPolygonTool, 
			     tool = new PolygonTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idEllipseTool, 
			     tool = new OvalTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idTextTool, 
			     tool = new TextTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idZoomTool, 
			     tool = new ZoomTool (&cmdHistory));
  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
		    this, SLOT(showCurrentMode(const char*)));
  tcontroller->toolSelected (0);

  canvas->setToolController (tcontroller);
  grid->activate ();
  w->show ();
  mainWidget = w;
}

void KIllustratorView::showCurrentMode (const char* msg) {
    //  statusbar->changeItem (msg, 2);
}

// XXXXXXXXXXXXXXXXXXX
void KIllustratorView::setupColorToolbar () {
#if 0
    m_vToolBarFactory = m_vPartShell->toolBarFactory ();
    if (! CORBA::is_nil (m_vToolBarFactory)) {
	m_rToolBarColors = 
	    m_vToolBarFactory->createToolBar (this, 
					      CORBA::string_dup 
					      (i18n ("Colors")));

	for (int i = 0; i < 18; i++) {
	  CORBA::Long id;
	  id = m_rToolBarColors->insertColorButton (colorPalette[i]->red (),
						    colorPalette[i]->green (),
						    colorPalette[i]->blue ());
	  if (i == 0)
	    m_idFirstColor = id;
	}

	m_rToolBarColors->connect ("rightPressed", this, "setFillColor");
	m_rToolBarColors->connect ("pressed", this, "setPenColor");
	m_rToolBarColors->setPos(KToolBar::Right);
    }
#endif
}

CORBA::Long KIllustratorView::addToolButton (const char* pictname,
					     const char* tooltip) {
  CORBA::Long id;
#if 0
  QString path = kapp->kde_datadir ().copy ();
  path += "/killustrator/pics/";
  path += pictname;
  QString pix = loadPixmap (path);
  id = m_rToolBarTools->insertLRButton (CORBA::string_dup (pix),
				      CORBA::string_dup (tooltip), 0L, 0L);
#endif
  return id;
}

// XXXXXXXXXXXXXXXXXXX
void KIllustratorView::setupToolsToolbar () {
#if 0
    m_vToolBarFactory = m_vPartShell->toolBarFactory ();
    if (! CORBA::is_nil (m_vToolBarFactory)) {
	m_rToolBarTools = 
	    m_vToolBarFactory->createToolBar (this, 
					      CORBA::string_dup 
					      (i18n ("Tools")));
	m_rToolBarTools->connect ("rightPressed", this, "configureTool");
	m_rToolBarTools->connect ("pressed", this, "activateTool");
	m_idSelectionTool = addToolButton ("selecttool.xpm", 
					   i18n("Selection Mode"));
	m_idEditPointTool = addToolButton ("pointtool.xpm", 
					   i18n("Edit Point"));
	m_idFreeHandTool = addToolButton ("freehandtool.xpm", 
					  i18n("Create FreeHand Line"));
	m_idPolylineTool = addToolButton ("linetool.xpm", 
					  i18n("Create Polyline"));
	m_idBezierTool = addToolButton ("beziertool.xpm", 
					i18n("Create Bezier Curve"));
	m_idRectangleTool = addToolButton ("recttool.xpm", 
					   i18n("Create Rectangle"));
	m_idPolygonTool = addToolButton ("polygontool.xpm", 
					 i18n("Create Polygon"));
	m_idEllipseTool = addToolButton ("ellipsetool.xpm", 
					 i18n("Create Ellipse"));
	m_idTextTool = addToolButton ("texttool.xpm", 
				      i18n("Create/Edit Text"));
	m_idZoomTool = addToolButton ("zoomtool.xpm", i18n("Zoom In"));
	
	m_rToolBarTools->setButton (m_idSelectionTool, true);
	m_idActiveTool = m_idSelectionTool;

	m_rToolBarTools->setPos(KToolBar::Left);
    }
#endif
}

void KIllustratorView::cleanUp () {
  if (m_bIsClean)
    return;

  QListIterator<KIllustratorFrame> it (m_lstFrames);
  for (; it.current () != 0L; ++it)
    it.current ()->detach ();

  OpenParts::MenuBarManager_var menu_bar_manager = 
    m_vMainWindow->menuBarManager ();
  if (! CORBA::is_nil (menu_bar_manager))
    menu_bar_manager->unregisterClient (id ());

  OpenParts::ToolBarManager_var tool_bar_manager = 
    m_vMainWindow->toolBarManager ();
  if (! CORBA::is_nil (tool_bar_manager))
    tool_bar_manager->unregisterClient (id ());

  m_pDoc->removeView (this);

  KoViewIf::cleanUp();
}

void KIllustratorView::construct () {
  resizeEvent (0L);
}

void KIllustratorView::newView () {
  KIllustratorShell* shell = new KIllustratorShell ();
  shell->show ();
  shell->setDocument (m_pDoc);
}

void KIllustratorView::setUndoStatus(bool undoPossible, bool redoPossible)
{
#if 0
  // we do this " " trick to avoid double translation of "Undo" and "Undo "
  m_rMenuBar->setItemEnabled(m_idMenuEdit_Undo, undoPossible);
  
  QString label = i18n("Undo");
  if (undoPossible) 
      label += " " + cmdHistory.getUndoName();
  m_rMenuBar->changeItem(label, m_idMenuEdit_Undo);
  
  m_rMenuBar->setItemEnabled(m_idMenuEdit_Redo, redoPossible);
  
  label = i18n("Redo");
  if (redoPossible)
      label += " " + cmdHistory.getRedoName();
  
  m_rMenuBar->changeItem(label, m_idMenuEdit_Redo);
#endif
}

void KIllustratorView::resizeEvent (QResizeEvent* ) {
  if (mainWidget) {
    mainWidget->resize (width (), height ()); 
    if (m_bShowGUI) {
      if (m_bShowRulers) {
	// draw rulers
	hRuler->show ();
	vRuler->show ();
      }
      viewport->showScrollBars ();
    }
    else {
      hRuler->hide ();
      vRuler->hide ();
      viewport->hideScrollBars ();
      grid->activate ();
    }
  }
}

#if 0
void KIllustratorView::setMode (OPParts::Part::Mode m) {
  Part_impl::setMode (m);
  if (mode () == OPParts::Part::ChildMode && ! m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;
}

void KIllustratorView::setFocus (CORBA::Boolean m) {
  Part_impl::setFocus (m);
  bool old = m_bShowGUI;

  if (mode () == OPParts::Part::ChildMode && ! m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if (m)
    activateTool (m_idSelectionTool);

  if (! m && m_pDoc != 0L) {
    // we lost the focus, so deselect all objects
    m_pDoc->unselectAllObjects ();
  }

  if (old != m_bShowGUI)
    resizeEvent (0L);
}
#endif

void KIllustratorView::showTransformationDialog (int id) {
  TransformationDialog *transformationDialog = 
    new TransformationDialog (&cmdHistory);
  QObject::connect (m_pDoc, SIGNAL (selectionChanged ()), 
		    transformationDialog, SLOT (update ()));
  transformationDialog->setDocument (m_pDoc);
  transformationDialog->showTab (id);
}


CORBA::Boolean KIllustratorView::printDlg () {
  return false;
}

void KIllustratorView::editUndo () {
  cmdHistory.undo ();
  //  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idSelectionTool);
}

void KIllustratorView::editRedo () {
  cmdHistory.redo ();
  //  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idSelectionTool);
}

void KIllustratorView::editCut () {
  cmdHistory.addCommand (new CutCmd (m_pDoc), true);
}

void KIllustratorView::editCopy () {
  cmdHistory.addCommand (new CopyCmd (m_pDoc), true);
}

void KIllustratorView::editPaste () {
  cmdHistory.addCommand (new PasteCmd (m_pDoc), true);
}

void KIllustratorView::editSelectAll () {
  m_pDoc->selectAllObjects ();
}

void KIllustratorView::editDelete () {
  cmdHistory.addCommand (new DeleteCmd (m_pDoc), true);
}

void KIllustratorView::editInsertOject () {
}

void KIllustratorView::editProperties () {
  int result = 1;

  if (m_pDoc->selectionIsEmpty ()) {
    result = KMsgBox::yesNo (this, "Warning", 
		    i18n ("This action will set the default\nproperties for new objects !\nWould you like to do it ?"),
		    KMsgBox::QUESTION, i18n ("Yes"), i18n ("No"));
  }
  if (result == 1)
    PropertyEditor::edit (&cmdHistory, m_pDoc);
}

void KIllustratorView::toggleRuler () {
  m_bShowRulers = !m_bShowRulers;
  //  m_rMenuBar->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);
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
  AlignmentDialog::alignSelection (m_pDoc, &cmdHistory);
}

void KIllustratorView::arrangeToFront () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc, RP_ToFront), true);
}

void KIllustratorView::arrangeToBack () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc, RP_ToBack), true);
}

void KIllustratorView::arrangeOneForward () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc, RP_ForwardOne), true);
}

void KIllustratorView::arrangeOneBack () {
  cmdHistory.addCommand (new ReorderCmd (m_pDoc, RP_BackwardOne), true);
}

void KIllustratorView::arrangeGroup () {
  cmdHistory.addCommand (new GroupCmd (m_pDoc), true);
}

void KIllustratorView::arrangeUngroup () {
  cmdHistory.addCommand (new UngroupCmd (m_pDoc), true);
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
  //  m_rMenuBar->setItemChecked (m_idMenuView_Grid, gridIsShown);
}

void KIllustratorView::setupGrid () {
  GridDialog::setupGrid (canvas);
}

void KIllustratorView::alignToGrid () {
  bool snap = ! canvas->snapToGrid ();
  canvas->snapToGrid (snap);
  //  m_rMenuBar->setItemChecked (m_idMenuLayout_AlignToGrid, snap);
}

void KIllustratorView::configureTool (CORBA::Long id) {
  tcontroller->configureTool (id);
}

void KIllustratorView::activateTool (CORBA::Long id) {
  //  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = id);
}

void KIllustratorView::setPenColor (CORBA::Long id) {
  int idx = id - m_idFirstColor;
  GObject::OutlineInfo oInfo;
  oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;
  oInfo.color = *(colorPalette[idx]);
  oInfo.style = (idx == 0 ? NoPen : SolidLine);
  
  GObject::FillInfo fInfo;
  fInfo.mask = 0;
    
  if (! m_pDoc->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc, oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result = 
      KMsgBox::yesNo (this, "Warning", 
		      i18n ("This action will set the default\nproperties for new objects !\nWould you like to do it ?"),
		      KMsgBox::QUESTION, i18n ("Yes"), i18n ("No"));
    if (result == 1)
      GObject::setDefaultOutlineInfo (oInfo);
  }
}

void KIllustratorView::setFillColor (CORBA::Long id) {
  int idx = id - m_idFirstColor;
  GObject::OutlineInfo oInfo;
  oInfo.mask = 0;
  
  GObject::FillInfo fInfo;
  fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  fInfo.color = *(colorPalette[idx]);
  fInfo.fstyle = (idx == 0 ? GObject::FillInfo::NoFill :
		GObject::FillInfo::SolidFill);

  if (! m_pDoc->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc, oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result = 
      KMsgBox::yesNo (this, "Warning", 
		      i18n ("This action will set the default\nproperties for new objects !\nWould you like to do it ?"),
		      KMsgBox::QUESTION, i18n ("Yes"), i18n ("No"));
    if (result == 1)
      GObject::setDefaultFillInfo (fInfo);
  }
}

void KIllustratorView::editLayers () {
  if (!layerDialog) 
    layerDialog = new LayerDialog ();
  layerDialog->manageDocument (m_pDoc);
  layerDialog->show ();
}
