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

#include <utils.h>
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

QList<GObject> KIllustratorView::clipboard;

KIllustratorChildFrame::KIllustratorChildFrame (KIllustratorView* view, 
						KIllustratorChild* child) :
PartFrame_impl (view) {
  m_pView = view;
  m_pChild = child;
}

KIllustratorView::KIllustratorView (QWidget* parent, const char* name, 
				    KIllustratorDocument* doc) :
QWidget (parent), View_impl (), KIllustrator::View_skel () {
  setWidget (this);
  Control_impl::setFocusPolicy (OPControls::Control::ClickFocus);

  m_lstFrames.setAutoDelete (true);
  m_pDoc = doc;
  m_bShowGUI = true;
  m_bShowRulers = true;
  mainWidget = 0L;
  viewport = 0L;
  transformationDialog = 0L;
  // restore default settings
  PStateManager* pmgr = PStateManager::instance ();
}

KIllustratorView::~KIllustratorView () {
  cleanUp ();
}

void KIllustratorView::createGUI () {
  cout << "create GUI ..." << endl;
  setupMenu ();
  setupMainToolbar ();
  setupToolsToolbar ();
  setupColorToolbar ();
  setupCanvas ();
}

void KIllustratorView::setupCanvas () {
  cout << "create Canvas ..." << endl;
  QWidget *w = new QWidget (this);
  w->resize (500, 500);
  
  grid = new QGridLayout (w, 2, 2);
  
  hRuler = new Ruler (Ruler::Horizontal, Ruler::Point, w);
  vRuler = new Ruler (Ruler::Vertical, Ruler::Point, w);
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

  //  cpalette = new ColorSelectionPalette (18, w);
  //  QObject::connect (cpalette, SIGNAL(penColorSelected(const QBrush&)),
  //	   this, SLOT(setPenColor(const QBrush&)));
  //  QObject::connect (cpalette, SIGNAL(fillColorSelected(const QBrush&)),
  //	   this, SLOT(setFillColor(const QBrush&)));
  //  hlayout->addWidget (cpalette, 0, AlignTop);
  
//  setView (w);
  
  tcontroller = new ToolController (this);
  Tool* tool;
  tcontroller->registerTool (m_idSelectionTool, 
			     tool = new SelectionTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idEditPointTool, 
			     tool = new EditPointTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idPolylineTool, 
			     tool = new PolylineTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idBezierTool, 
			     tool = new BezierTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idRectangleTool, 
			     tool = new RectangleTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idPolygonTool, 
			     tool = new PolygonTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idEllipseTool, 
			     tool = new OvalTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idTextTool, 
			     tool = new TextTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (m_idZoomTool, 
			     tool = new ZoomTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->toolSelected (0);

  //  QObject::connect (tpalette, SIGNAL (toolSelected (int)), tcontroller,
  // 	   SLOT(toolSelected (int)));
  // QObject::connect (tpalette, SIGNAL (toolConfigActivated (int)), tcontroller,
  //	   SLOT(configureTool (int)));
  canvas->setToolController (tcontroller);
  grid->activate ();
  w->show ();
  mainWidget = w;
//  resizeEvent (0L);
}

void KIllustratorView::setupMenu () {
  m_vMenuBarFactory = m_vPartShell->menuBarFactory ();
  if (! CORBA::is_nil (m_vMenuBarFactory)) {
    m_rMenuBar = m_vMenuBarFactory->createMenuBar (this);

    /* ------------- Edit Menu ------------- */
    m_idMenuEdit = m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&Edit")));
  
    m_idMenuEdit_Undo = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Undo")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editUndo"));
    m_idMenuEdit_Redo = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Redo")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editRedo"));
    m_rMenuBar->insertSeparator (m_idMenuEdit);
    m_idMenuEdit_Copy = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Copy")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editCopy"));
    m_idMenuEdit_Paste = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Paste")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editPaste"));
    m_idMenuEdit_Cut = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("C&ut")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editCut"));

    m_rMenuBar->insertSeparator (m_idMenuEdit);

    m_idMenuEdit_Delete = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Delete")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editDelete"));

    m_rMenuBar->insertSeparator (m_idMenuEdit);

    m_idMenuEdit_SelectAll = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Select All")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editSelectAll"));

    m_rMenuBar->insertSeparator (m_idMenuEdit);

    m_idMenuEdit_InsertObject = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Insert Object...")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editInsertObject"));
    m_idMenuEdit_Properties = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Pr&operties")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editProperties"));

    /* ------------- View Menu ------------- */
    m_idMenuView = m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&View")));
    m_idMenuView_Outline =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Outline")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("viewOutline"));
    m_rMenuBar->setCheckable (m_idMenuView, true);

    m_idMenuView_Draft =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Draft")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("viewDraft"));

    m_idMenuView_Normal =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Normal")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("viewNormal"));

    m_rMenuBar->insertSeparator (m_idMenuView);

    m_idMenuView_Ruler =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Ruler")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("toggleRuler"));
    m_rMenuBar->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);

    m_idMenuView_Grid =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Grid")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("toggleGrid"));
    m_rMenuBar->setItemChecked (m_idMenuView_Grid, false);
      
    /* ------------- Layout Menu ------------- */
    m_idMenuLayout = 
      m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&Layout")));
    m_idMenuLayout_InsertPage =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Insert Page")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("insertPage"));
    m_idMenuLayout_RemovePage =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Remove Page")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("removePage"));
    m_idMenuLayout_GotoPage =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Go to Page")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("gotoPage"));
    m_idMenuLayout_PageLayout =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Page Layout")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("setupPage"));

    m_rMenuBar->insertSeparator (m_idMenuLayout);

    m_idMenuLayout_SetupGrid =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Setup Grid")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("setupGrid"));

    m_idMenuLayout_AlignToGrid =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Align to Grid")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("alignToGrid"));
    m_rMenuBar->setCheckable (m_idMenuLayout_AlignToGrid, true);
    m_rMenuBar->setItemChecked (m_idMenuLayout_AlignToGrid, false);

    /* ------------- Arrange Menu ------------- */
    m_idMenuArrange = 
      m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&Arrange")));
    m_idMenuTransform =
      m_rMenuBar->insertSubMenu (CORBA::string_dup (i18n ("Transform")),
				 m_idMenuArrange);
    m_idMenuArrange_Align =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Align")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeAlign"));
    m_idMenuArrange_ToFront =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("To Front")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeToFront"));
    m_idMenuArrange_ToBack =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("To Back")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeToBack"));
    m_idMenuArrange_1Forward =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Forward One")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeOneForward"));
    m_idMenuArrange_1Back =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Back One")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeOneBack"));
    m_rMenuBar->insertSeparator (m_idMenuArrange);
    m_idMenuArrange_Group =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Group")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeGroup"));
    m_idMenuArrange_Ungroup =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Ungroup")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeUngroup"));

    /* ------------- Arrange->Transform Menu ------------- */
    m_idMenuTransform_Position =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Position")), 
			      m_idMenuTransform, this, 
			      CORBA::string_dup ("transformPosition"));
    m_idMenuTransform_Dimension =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Dimension")), 
			      m_idMenuTransform, this, 
			      CORBA::string_dup ("transformDimension"));
    m_idMenuTransform_Rotation =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Rotation")), 
			      m_idMenuTransform, this, 
			      CORBA::string_dup ("transformRotation"));
    m_idMenuTransform_Mirror =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Mirror")), 
			      m_idMenuTransform, this, 
			      CORBA::string_dup ("transformMirror"));


    /* ------------- Extras Menu ------------- */
    m_idMenuExtras = 
      m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("Ex&tras")));

    /* ------------- Help Menu ------------- */

    CORBA::Long id = m_rMenuBar->helpMenuId ();
    m_idMenuHelp_About = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("About")), 
			      id, this, 
			      CORBA::string_dup ("helpAbout"));
  }
}

void KIllustratorView::setupColorToolbar () {
}

void KIllustratorView::setupMainToolbar () {
}

CORBA::Long KIllustratorView::addToolButton (const char* pictname,
					     const char* tooltip,
					     const char* func) {
  CORBA::Long id;

  QString path = kapp->kde_datadir ().copy ();
  path += "/killustrator/pics/";
  path += pictname;
  QString pix = loadPixmap (path);
  id = m_rToolBarTools->insertButton (CORBA::string_dup (pix),
				      CORBA::string_dup (i18n (tooltip)),
				      this,
				      CORBA::string_dup (func));
  m_rToolBarTools->setToggle (id, true);
  return id;
}

void KIllustratorView::setupToolsToolbar () {
  m_vToolBarFactory = m_vPartShell->toolBarFactory ();
  if (! CORBA::is_nil (m_vToolBarFactory)) {
    m_rToolBarTools = 
      m_vToolBarFactory->createToolBar (this, 
					CORBA::string_dup (i18n ("Tools")));
    m_idSelectionTool = addToolButton ("selecttool.xpm", "Selection Mode",
				       "activateSelectionTool");
    m_idEditPointTool = addToolButton ("pointtool.xpm", "Edit Point",
				       "activateEditPointTool");
    m_idPolylineTool = addToolButton ("linetool.xpm", "Create Polyline",
				      "activateLineTool");
    m_idBezierTool = addToolButton ("beziertool.xpm", "Create Bezier Curve",
				    "activateBezierTool");
    m_idRectangleTool = addToolButton ("recttool.xpm", "Create Rectangle",
				       "activateRectangleTool");
    m_idPolygonTool = addToolButton ("polygontool.xpm", "Create Polygon",
				     "activatePolygonTool");
    m_idEllipseTool = addToolButton ("ellipsetool.xpm", "Create Ellipse",
				     "activateEllipseTool");
    m_idTextTool = addToolButton ("texttool.xpm", "Create/Edit Text",
				  "activateTextTool");
    m_idZoomTool = addToolButton ("zoomtool.xpm", "Zoom In",
				  "activateZoomTool");

    m_rToolBarTools->setButton (m_idSelectionTool, true);
    m_idActiveTool = m_idSelectionTool;
  }
}

void KIllustratorView::cleanUp () {
  if (m_bIsClean)
    return;

  m_pDoc->removeView (this);
  m_lstFrames.clear ();
  
  View_impl::cleanUp ();
}

void KIllustratorView::construct () {
  cout << "KIllustratorView::construct ()" << endl;
  resizeEvent (0L);
}

void KIllustratorView::newView () {
  KIllustratorShell* shell = new KIllustratorShell;
  shell->enableMenuBar ();
  shell->PartShell_impl::enableStatusBar ();
  shell->enableToolBars ();
  shell->show ();
  shell->setDocument (m_pDoc);

  CORBA::release (shell);
}

void KIllustratorView::resizeEvent (QResizeEvent* ) {
  if (mainWidget) {
    cout << "KIllustratorView::resize mainWidget to: " 
	 << width () << ", " << height () 
	 << " : showGUI = " << m_bShowGUI << endl;
    mainWidget->resize (width (), height ()); 
    if (m_bShowGUI) {
      hRuler->show ();
      vRuler->show ();
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

  if (! m && m_pDoc != 0L) {
    // we lost the focus, so deselect all objects
    m_pDoc->unselectAllObjects ();
  }

  if (old != m_bShowGUI)
    resizeEvent (0L);
}

void KIllustratorView::showTransformationDialog (int id) {
  if (transformationDialog == 0L) {
    transformationDialog = new TransformationDialog (&cmdHistory);
    QObject::connect (m_pDoc, SIGNAL (selectionChanged ()), 
                      transformationDialog, SLOT (update ()));
  }
  transformationDialog->setDocument (m_pDoc);
  transformationDialog->showTab (id);
}


CORBA::Boolean KIllustratorView::printDlg () {
  return false;
}

void KIllustratorView::editUndo () {
  cmdHistory.undo ();
  activateSelectionTool ();
}

void KIllustratorView::editRedo () {
  cmdHistory.redo ();
  activateSelectionTool ();
}

void KIllustratorView::editCut () {
  cmdHistory.addCommand (new CutCmd (m_pDoc, clipboard), true);
}

void KIllustratorView::editCopy () {
  cmdHistory.addCommand (new CopyCmd (m_pDoc, clipboard), true);
}

void KIllustratorView::editPaste () {
  cmdHistory.addCommand (new PasteCmd (m_pDoc, clipboard), true);
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
  PropertyEditor::edit (&cmdHistory, m_pDoc);
}

void KIllustratorView::activateSelectionTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idSelectionTool);
}

void KIllustratorView::activateEditPointTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idEditPointTool);
}

void KIllustratorView::activateLineTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idPolylineTool);
}

void KIllustratorView::activateBezierTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idBezierTool);
}

void KIllustratorView::activateRectangleTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idRectangleTool);
}

void KIllustratorView::activatePolygonTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idPolygonTool);
}

void KIllustratorView::activateEllipseTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idEllipseTool);
}

void KIllustratorView::activateTextTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idTextTool);
}

void KIllustratorView::activateZoomTool () {
  m_rToolBarTools->setButton (m_idActiveTool, false);
  tcontroller->toolSelected (m_idActiveTool = m_idZoomTool);
}

void KIllustratorView::toggleRuler () {
  m_bShowRulers = !m_bShowRulers;
  m_rMenuBar->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);
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
  m_rMenuBar->setItemChecked (m_idMenuView_Grid, gridIsShown);
}

void KIllustratorView::setupGrid () {
  GridDialog::setupGrid (canvas);
}

void KIllustratorView::alignToGrid () {
  bool snap = ! canvas->snapToGrid ();
  canvas->snapToGrid (snap);
  m_rMenuBar->setItemChecked (m_idMenuLayout_AlignToGrid, snap);
}

