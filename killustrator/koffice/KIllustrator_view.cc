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

#include <openparts_ui.h>
#include <opUIUtils.h>
#include <koPartSelectDia.h>

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

  // create frames for all embedded parts
  assert (m_pDoc != 0L);
  m_lstFrames.clear ();

  vector<GLayer*>::const_iterator i = m_pDoc->getLayers ().begin ();
  for (; i != m_pDoc->getLayers ().end (); i++) {
    GLayer* layer = *i;
    list<GObject*>& contents = layer->objects ();
    for (list<GObject*>::iterator oi = contents.begin ();
	 oi != contents.end (); oi++) {
      if ((*oi)->isA ("GPart")) {
	GPart *part = (GPart *) *oi;
	insertPartSlot (part->getChild (), part);
      }
    }
  }
}

KIllustratorView::~KIllustratorView () {
  cout << "~KIllustratorView ()" << endl;
  cleanUp ();
}

void KIllustratorView::createGUI () {
  setupCanvas ();
  setupPopups ();
  setUndoStatus (false, false);
  QObject::connect (&cmdHistory, SIGNAL(changed(bool, bool)),
		    SLOT(setUndoStatus(bool, bool)));
}

void KIllustratorView::setupPopups () {
  objMenu = new QPopupMenu ();
  objMenu->insertItem (i18n ("Copy"), this, SLOT (editCopySlot ()));
  objMenu->insertItem (i18n ("Cut"), this, SLOT (editCutSlot ()));
  objMenu->insertSeparator ();
  objMenu->insertItem (i18n ("Properties"), this,
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

  CORBA::WString_var text;
  
  // Menu: Edit
  text = Q2C( i18n ("&Edit") );
  menubar->insertMenu (text, m_vMenuEdit, -1, -1);
  text = Q2C( i18n ("Undo") );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem (text, this,
					       "editUndo", 0);
  text = Q2C( i18n ("Redo") );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem (text, this,
					       "editRedo", 0);
  m_vMenuEdit->insertSeparator (-1);
  text = Q2C( i18n ("&Copy") );
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem (text, this,
					   "editCopy", 0);
  text = Q2C( i18n ("&Paste") );
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem (text, this,
					    "editPaste", 0);
  text = Q2C( i18n ("C&ut") );
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem (text, this,
					      "editCut", 0);
  m_vMenuEdit->insertSeparator (-1);
  text = Q2C( i18n ("Duplicate") );
  m_idMenuEdit_Duplicate = m_vMenuEdit->insertItem (text, this,
						 "editDuplicate", 0);
  text = Q2C( i18n ("&Delete") );
  m_idMenuEdit_Delete = m_vMenuEdit->insertItem (text, this,
						 "editDelete", 0);
  m_vMenuEdit->insertSeparator (-1);
  text = Q2C( i18n ("&Select All") );
  m_idMenuEdit_SelectAll = m_vMenuEdit->insertItem (text, this,
						    "editSelectAll", 0);
  m_vMenuEdit->insertSeparator (-1);

  text = Q2C( i18n ("&Insert") );
  m_vMenuEdit->insertItem8 (text, m_vMenuInsert, -1, -1);

  text = Q2C( i18n ("Pr&operties") );
  m_idMenuEdit_Properties =
    m_vMenuEdit->insertItem (text, this,
			     "editProperties", 0);

  // Menu: Edit->Insert
  text = Q2C( i18n ("&Object...") );
  m_idMenuInsert_Object =
    m_vMenuInsert->insertItem (text, this,
			       "editInsertObject", 0);
  text = Q2C( i18n ("&Clipart...") );
  m_idMenuInsert_Clipart =
    m_vMenuInsert->insertItem (text, this,
			       "editInsertClipart", 0);
  text = Q2C( i18n ("&Bitmap...") );
  m_idMenuInsert_Bitmap =
    m_vMenuInsert->insertItem (text, this,
			       "editInsertBitmap", 0);

  // Menu: View
  text = Q2C( i18n ("&View") );
  menubar->insertMenu (text, m_vMenuView, -1, -1);
  text = Q2C( i18n ("Outline") );
  m_idMenuView_Outline =
    m_vMenuView->insertItem (text, this, "viewOutline", 0);
  m_vMenuView->setCheckable (true);
  text = Q2C( i18n ("Normal") );
  m_idMenuView_Normal =
    m_vMenuView->insertItem (text, this, "viewNormal", 0);
  m_vMenuView->insertSeparator (-1);
  text = Q2C( i18n ("Layers") );
  m_idMenuView_Layers =
    m_vMenuView->insertItem (text, this, "editLayers", 0);
  m_vMenuView->insertSeparator (-1);
  text = Q2C( i18n ("Ruler") );
  m_idMenuView_Ruler =
    m_vMenuView->insertItem (text, this, "toggleRuler", 0);
  m_vMenuView->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);
  text = Q2C( i18n ("Grid") );
  m_idMenuView_Grid =
    m_vMenuView->insertItem (text, this, "toggleGrid", 0);
  m_vMenuView->setItemChecked (m_idMenuView_Grid, false);
  text = Q2C( i18n ("Helplines") );
  m_idMenuView_Helplines =
    m_vMenuView->insertItem (text, this, "toggleHelplines", 0);
  m_vMenuView->setItemChecked (m_idMenuView_Helplines, false);

  // Menu: Layout
  text = Q2C( i18n ("&Layout") );
  menubar->insertMenu (text, m_vMenuLayout, -1, -1);
  text = Q2C( i18n ("Page Layout") );
  m_idMenuLayout_PageLayout =
    m_vMenuLayout->insertItem (text, this, "setupPage", 0);
  m_vMenuLayout->insertSeparator (-1);
  text = Q2C( i18n ("Setup Grid") );
  m_idMenuLayout_SetupGrid =
    m_vMenuLayout->insertItem (text, this, "setupGrid", 0);
  text = Q2C( i18n ("Setup Helplines") );
  m_idMenuLayout_SetupHelplines =
    m_vMenuLayout->insertItem (text, this,
			       "setupHelplines", 0);
  m_vMenuLayout->insertSeparator (-1);
  text = Q2C( i18n ("Align to Grid") );
  m_idMenuLayout_AlignToGrid =
    m_vMenuLayout->insertItem (text, this, "alignToGrid", 0);
  m_vMenuLayout->setCheckable (true);
  m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToGrid, false);
  text = Q2C( i18n ("Align to Helplines") );
  m_idMenuLayout_AlignToHelplines =
    m_vMenuLayout->insertItem (text, this,
			       "alignToHelplines", 0);
  m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToHelplines, false);

  // Menu: Arrange
  text = Q2C( i18n ("&Arrange") );
  menubar->insertMenu (text, m_vMenuArrange, -1, -1);
  text = Q2C( i18n ("Transform") );
  m_vMenuArrange->insertItem8 (text, m_vMenuTransform, -1, -1);
  text = Q2C( i18n ("Align") );
  m_idMenuArrange_Align = m_vMenuArrange->insertItem (text, this,
						      "arrangeAlign", 0);
  text = Q2C( i18n ("To Front") );
  m_idMenuArrange_ToFront = m_vMenuArrange->insertItem (text,
							this,
							"arrangeToFront", 0);
  text = Q2C( i18n ("To Back") );
  m_idMenuArrange_ToBack = m_vMenuArrange->insertItem (text, this,
				"arrangeToBack", 0);
  text = Q2C( i18n ("Forward One") );
  m_idMenuArrange_1Forward =
    m_vMenuArrange->insertItem (text, this,
				"arrangeOneForward", 0);
  text = Q2C( i18n ("Back One") );
  m_idMenuArrange_1Back = m_vMenuArrange->insertItem (text, this,
						      "arrangeOneBack", 0);
  m_vMenuArrange->insertSeparator (-1);
  text = Q2C( i18n ("Group") );
  m_idMenuArrange_Group = m_vMenuArrange->insertItem (text, this,
						      "arrangeGroup", 0);
  text = Q2C( i18n ("Ungroup") );
  m_idMenuArrange_Ungroup = m_vMenuArrange->insertItem (text,
							this,
							"arrangeUngroup", 0);
  m_vMenuArrange->insertSeparator (-1);
  text = Q2C( i18n ("Text along path") );
  m_idMenuArrange_TextAlongPath =
    m_vMenuArrange->insertItem (text,
				this,
				"arrangeTextAlongPath", 0);

  // Menu: Arrange->Transform
  text = Q2C( i18n ("Position") );
  m_idMenuTransform_Position =
    m_vMenuTransform->insertItem (text, this,
				  "transformPosition", 0);
  text = Q2C( i18n ("Dimension") );
  m_idMenuTransform_Dimension =
    m_vMenuTransform->insertItem (text, this,
				  "transformDimension", 0);
  text = Q2C( i18n ("Rotation") );
  m_idMenuTransform_Rotation =
    m_vMenuTransform->insertItem (text, this,
				  "transformRotation", 0);
  text = Q2C( i18n ("Mirror") );
  m_idMenuTransform_Mirror =
    m_vMenuTransform->insertItem (text, this,
				  "transformMirror", 0);

  // Menu: Extras
  text = Q2C( i18n ("&Extras") );
  menubar->insertMenu (text, m_vMenuExtras, -1, -1);
  text = Q2C( i18n ("Scripts") );
  m_idMenuExtras_Scripts =
    m_vMenuExtras->insertItem (text, this,
				  "showScripts", 0);

  // Menu: Help
  m_vMenuHelp = menubar->helpMenu ();
  if (CORBA::is_nil (m_vMenuHelp)) {
    menubar->insertSeparator (-1);
    text = Q2C( i18n ("&Help") );
    menubar->setHelpMenu (menubar->insertMenu (text,
					       m_vMenuHelp, -1, -1));
  }
  return true;
}

bool KIllustratorView::mappingCreateToolbar (OpenPartsUI::ToolBarFactory_ptr
					     factory) {
  if (CORBA::is_nil (factory)) {
    m_vToolBarTools = 0L;
    m_vToolBarEdit = 0L;
    m_vColorBar = 0L;
    m_vToolBarEditPoint = 0L;
    return true;
  }

  m_vToolBarTools = factory->create (OpenPartsUI::ToolBarFactory::Transient);
  m_vToolBarTools->setFullWidth (false);
  QString tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/selecttool.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap (tmp);
  m_idSelectionTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_SELECT,
				    SIGNAL (clicked ()), this,
				    "toolSelection", true,
				    i18n ("Selection Mode"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_SELECT, true);
  m_vToolBarTools->setButton (ID_TOOL_SELECT, true);
  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/pointtool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idEditPointTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_EDITPOINT,
				    SIGNAL (clicked ()), this,
				    "toolEditPoint", true,
				    i18n ("Edit Point"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_EDITPOINT, true);

  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/freehandtool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idFreeHandTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_FREEHAND,
				    SIGNAL (clicked ()), this,
				    "toolFreehandLine", true,
				    i18n ("Create FreeHand Line"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_FREEHAND, true);

  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/linetool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idPolylineTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_LINE,
				    SIGNAL (clicked ()), this,
				    "toolPolyline", true,
				    i18n ("Create Polyline"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_LINE, true);

  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/beziertool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idBezierTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_BEZIER,
				    SIGNAL (clicked ()), this,
				    "toolBezier", true,
				    i18n ("Create Bezier Curve"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_BEZIER, true);


  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/recttool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idRectangleTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_RECTANGLE,
				    SIGNAL (clicked ()), this,
				    "toolRectangle", true,
				    i18n ("Create Rectangle"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_RECTANGLE, true);


  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/polygontool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idPolygonTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_POLYGON,
				    SIGNAL (clicked ()), this,
				    "toolPolygon", true,
				    i18n ("Create Polygon"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_POLYGON, true);
  m_vToolBarTools->addConnection (ID_TOOL_POLYGON, SIGNAL(doubleClicked(int)),
				  this, "configPolygonTool");

  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/ellipsetool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idEllipseTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_ELLIPSE,
				    SIGNAL (clicked ()), this,
				    "toolEllipse", true,
				    i18n ("Create Ellipse"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_ELLIPSE, true);
  m_vToolBarTools->addConnection (ID_TOOL_ELLIPSE, SIGNAL(doubleClicked(int)),
				  this, "configEllipseTool");

  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/texttool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idTextTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_TEXT,
				    SIGNAL (clicked ()), this,
				    "toolText", true,
				    i18n ("Create/Edit Text"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_TEXT, true);

  tmp = kapp->kde_datadir().copy ();
  tmp += "/killustrator/pics/zoomtool.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idZoomTool =
    m_vToolBarTools->insertButton2 (pix, ID_TOOL_ZOOM,
				    SIGNAL (clicked ()), this,
				    "toolZoom", true,
				    i18n ("Zoom In"), -1);
  m_vToolBarTools->setToggle (ID_TOOL_ZOOM, true);

  m_vToolBarTools->setBarPos (OpenPartsUI::Left);
  m_vToolBarTools->enable (OpenPartsUI::Show);

  /*
   * ToolBar: Edit
   */

  m_vToolBarEdit = factory->create (OpenPartsUI::ToolBarFactory::Transient);
  m_vToolBarEdit->setFullWidth (false);

  pix = OPUIUtils::convertPixmap (ICON("undo.xpm"));
  m_idEditUndo =
    m_vToolBarEdit->insertButton2 (pix, ID_EDIT_UNDO, SIGNAL (clicked ()),
				   this, "editUndo", true, i18n ("Undo"), -1);
  //  m_vToolBarEdit->setItemEnabled (ID_EDIT_UNDO, false);

  pix = OPUIUtils::convertPixmap (ICON("redo.xpm"));
  m_idEditRedo =
    m_vToolBarEdit->insertButton2 (pix, ID_EDIT_REDO, SIGNAL (clicked ()),
				   this, "editRedo", true, i18n ("Redo"), -1);
  //  m_vToolBarEdit->setItemEnabled (ID_EDIT_REDO, false);
  m_vToolBarEdit->insertSeparator (-1);

  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/editcut.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idEditCut =
    m_vToolBarEdit->insertButton2 (pix, ID_EDIT_CUT, SIGNAL (clicked ()),
				   this, "editCut", true, i18n ("Cut"), -1);

  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/editcopy.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idEditCopy =
    m_vToolBarEdit->insertButton2 (pix, ID_EDIT_COPY, SIGNAL (clicked ()),
				   this, "editCopy", true, i18n ("Copy"), -1);

  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/editpaste.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idEditPaste =
    m_vToolBarEdit->insertButton2 (pix, ID_EDIT_PASTE, SIGNAL (clicked ()),
				   this, "editPaste", true,
				   i18n ("Paste"), -1);
  m_vToolBarEdit->insertSeparator (-1);

  tmp = kapp->kde_datadir ().copy ();
  tmp += "/kpresenter/toolbar/delete.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idEditDelete =
    m_vToolBarEdit->insertButton2 (pix, ID_EDIT_DELETE, SIGNAL (clicked ()),
				   this, "editDelete", true,
				   i18n ("Delete"), -1);
  m_vToolBarEdit->insertSeparator (-1);

  OpenPartsUI::StrList zoomSizes;
  zoomSizes.length ((int) zFactors.size ());
  for (int i = 0; i < (int) zFactors.size (); i++) {
    char buf[8];
    sprintf (buf, "%3.0f%%", zFactors[i] * 100);
    zoomSizes[i] = CORBA::string_dup (buf);
  }
  m_idEditZoom =
  m_vToolBarEdit->insertCombo (zoomSizes, ID_EDIT_ZOOM, true,
			       SIGNAL (activated (const QString &)),
			       this, "zoomSizeSelected", true,
			       i18n ("Zoom Factors"), 85, -1,
			       OpenPartsUI::AtBottom);
  m_vToolBarEdit->setCurrentComboItem (ID_EDIT_ZOOM, 1);
  m_vToolBarEdit->enable (OpenPartsUI::Show);

  /*
   * ColorBar
   */

  m_vColorBar =
    factory->createColorBar (OpenPartsUI::ToolBarFactory::Transient);
  m_vColorBar->setFullWidth (false);

  int i = 0;
  const QColor cpalette[] = { white, red, green, blue, cyan, magenta, yellow,
			      darkRed, darkGreen, darkBlue, darkCyan,
			      darkMagenta, darkYellow, white, lightGray,
			      gray, darkGray, black };

  for (i = 0; i < 18; i++)
     m_vColorBar->insertRgbColor (i, cpalette[i].red (),
				  cpalette[i].green (),
				  cpalette[i].blue (),
				  i != 0);

  m_vColorBar->addConnection (SIGNAL (fgColorSelected(int)), this,
			      "setPenColor");
  m_vColorBar->addConnection (SIGNAL (bgColorSelected(int)), this,
			      "setFillColor");

  m_vColorBar->setBarPos (OpenPartsUI::Right);
  m_vColorBar->enable (OpenPartsUI::Show);

  m_vToolBarEditPoint =
    factory->create (OpenPartsUI::ToolBarFactory::Transient);

  tmp = kapp->kde_datadir ().copy ();
  tmp += "/killustrator/pics/moveNode.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idMovePoint =
    m_vToolBarEditPoint->insertButton2 (pix, ID_TOOL_EP_MOVE,
					SIGNAL (clicked ()),
					this, "toolMovePoint", true,
					i18n ("Move Point"), -1);
  m_vToolBarEditPoint->setToggle (ID_TOOL_EP_MOVE, true);

  tmp = kapp->kde_datadir ().copy ();
  tmp += "/killustrator/pics/newNode.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idInsertPoint =
    m_vToolBarEditPoint->insertButton2 (pix, ID_TOOL_EP_INSERT,
					SIGNAL (clicked ()),
					this, "toolInsertPoint", true,
					i18n ("Insert Point"), -1);
  m_vToolBarEditPoint->setToggle (ID_TOOL_EP_INSERT, true);

  tmp = kapp->kde_datadir ().copy ();
  tmp += "/killustrator/pics/deleteNode.xpm";
  pix = OPUIUtils::loadPixmap (tmp);
  m_idRemovePoint =
    m_vToolBarEditPoint->insertButton2 (pix, ID_TOOL_EP_DELETE,
					SIGNAL (clicked ()),
					this, "toolRemovePoint", true,
					i18n ("Remove Point"), -1);
  m_vToolBarEditPoint->setToggle (ID_TOOL_EP_DELETE, true);

  m_vToolBarEditPoint->setBarPos (OpenPartsUI::Floating);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
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
  QObject::connect (canvas, SIGNAL(rightButtonAtSelectionClicked (int, int)),
	   this, SLOT(popupForSelection (int, int)));

  widget ()->setFocusPolicy (QWidget::StrongFocus);
  /*widget ()->*/setFocusProxy (canvas);

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

void KIllustratorView::showCurrentMode (const char* msg) {
    //  statusbar->changeItem (msg, 2);
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

void KIllustratorView::newView () {
  KIllustratorShell* shell = new KIllustratorShell ();
  shell->show ();
  shell->setDocument (m_pDoc);
}

void KIllustratorView::setUndoStatus(bool undoPossible, bool redoPossible)
{
  if (! CORBA::is_nil (m_vMenuEdit)) {
    // we do this " " trick to avoid double translation of "Undo" and "Undo "
    m_vMenuEdit->setItemEnabled (m_idMenuEdit_Undo, undoPossible);

    CORBA::WString_var text;
    
    QString label = i18n ("Undo");
    if (undoPossible)
      label += " " + cmdHistory.getUndoName ();
    text = Q2C( label );
    m_vMenuEdit->changeItemText (text, m_idMenuEdit_Undo);

    m_vMenuEdit->setItemEnabled (m_idMenuEdit_Redo, redoPossible);

    label = i18n ("Redo");
    if (redoPossible)
      label += " " + cmdHistory.getRedoName ();

    text = Q2C( label );
    m_vMenuEdit->changeItemText (text, m_idMenuEdit_Redo);
  }
}

void KIllustratorView::resizeEvent (QResizeEvent* ) {
  //  cout << "resizeEvent" << endl;
  if (mainWidget) {
    mainWidget->resize (width (), height ());
    if ((KoViewIf::hasFocus () || mode () == KOffice::View::RootMode) &&
	m_bShowGUI) {
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

void KIllustratorView::showTransformationDialog (int id) {
  TransformationDialog *transformationDialog =
    new TransformationDialog (&cmdHistory);
  QObject::connect (m_pDoc, SIGNAL (selectionChanged ()),
		    transformationDialog, SLOT (update ()));
  transformationDialog->setDocument (m_pDoc);
  transformationDialog->showTab (id);
}


CORBA::Boolean KIllustratorView::printDlg () {
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

void KIllustratorView::editDuplicate () {
  cmdHistory.addCommand (new DuplicateCmd (m_pDoc), true);
}

void KIllustratorView::editInsertObject () {
  m_pDoc->unselectAllObjects ();
  KoDocumentEntry docEntry = KoPartSelectDia::selectPart ();
  if (docEntry.name.isEmpty ())
    return;

  insertPartTool->setPartEntry (docEntry);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_INSERTPART);
}

void KIllustratorView::editInsertClipart () {
  QString fname = KFilePreviewDialog::getOpenFileName
    (QString::null, "*.wmf *.WMF | Windows Metafiles", this);
  if (! fname.isEmpty ()) {
    InsertClipartCmd *cmd = new InsertClipartCmd (m_pDoc,
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
    InsertPixmapCmd *cmd = new InsertPixmapCmd (m_pDoc,
						(const char *) fname);
    cmdHistory.addCommand (cmd, true);
  }
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
  m_vMenuView->setItemChecked (m_idMenuView_Ruler, m_bShowRulers);
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

void KIllustratorView::arrangeTextAlongPath () {
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_PATHTEXT);
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
  m_vMenuLayout->setItemChecked (m_idMenuView_Grid, gridIsShown);
}

void KIllustratorView::toggleHelplines () {
  bool linesAreShown = ! canvas->showHelplines ();
  canvas->showHelplines (linesAreShown);
  m_vMenuLayout->setItemChecked (m_idMenuView_Helplines, linesAreShown);
}

void KIllustratorView::setupGrid () {
  GridDialog::setupGrid (canvas);
}

void KIllustratorView::alignToGrid () {
  bool snap = ! canvas->snapToGrid ();
  canvas->snapToGrid (snap);
  m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToGrid, snap);
}

void KIllustratorView::setupHelplines () {
  HelplineDialog::setup (canvas);
}

void KIllustratorView::alignToHelplines () {
  bool snap = ! canvas->alignToHelplines ();
  canvas->alignToHelplines (snap);
  m_vMenuLayout->setItemChecked (m_idMenuLayout_AlignToHelplines, snap);
}

void KIllustratorView::setPenColor (CORBA::Long id) {
  CORBA::Short red, green, blue;
  CORBA::Boolean fill;
  if(! m_vColorBar->getRgbColor (id, red, green, blue, fill))
    return;

  GObject::OutlineInfo oInfo;
  oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;
  oInfo.color = QColor (red, green, blue);
  oInfo.style = fill ? SolidLine : NoPen;

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
  CORBA::Short red, green, blue;
  CORBA::Boolean fill;
  if(! m_vColorBar->getRgbColor (id, red, green, blue, fill))
    return;

  GObject::OutlineInfo oInfo;
  oInfo.mask = 0;

  GObject::FillInfo fInfo;
  fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  fInfo.color = QColor (red, green, blue);
  fInfo.fstyle = fill ? GObject::FillInfo::SolidFill :
    GObject::FillInfo::NoFill;

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

void KIllustratorView::toolSelection () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_SELECT);
}

void KIllustratorView::toolEditPoint () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  toolMovePoint ();
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_MOVE, true);
  m_vToolBarEditPoint->enable (OpenPartsUI::Show);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_EDITPOINT);
}

void KIllustratorView::toolFreehandLine () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_FREEHAND);
}

void KIllustratorView::toolPolyline () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_LINE);
}

void KIllustratorView::toolBezier () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_BEZIER);
}

void KIllustratorView::toolRectangle () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_RECTANGLE);
}

void KIllustratorView::toolPolygon () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_POLYGON);
}

void KIllustratorView::toolEllipse () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_ELLIPSE);
}

void KIllustratorView::toolText () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_TEXT);
}

void KIllustratorView::toolZoom () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_ZOOM);
}

void KIllustratorView::configPolygonTool () {
  tcontroller->configureTool (ID_TOOL_POLYGON);
}

void KIllustratorView::configEllipseTool () {
  tcontroller->configureTool (ID_TOOL_ELLIPSE);
}

void KIllustratorView::viewOutline () {
  canvas->setOutlineMode (true);
  m_vMenuView->setItemChecked (m_idMenuView_Outline, true);
  m_vMenuView->setItemChecked (m_idMenuView_Normal, false);
}

void KIllustratorView::viewNormal () {
  canvas->setOutlineMode (false);
  m_vMenuView->setItemChecked (m_idMenuView_Outline, false);
  m_vMenuView->setItemChecked (m_idMenuView_Normal, true);
}

void KIllustratorView::setupPage () {
  KoPageLayout pLayout = m_pDoc->pageLayout ();
  KoHeadFoot header;

  if (KoPageLayoutDia::pageLayout (pLayout, header,
				   FORMAT_AND_BORDERS))
    m_pDoc->setPageLayout (pLayout);
}

void KIllustratorView::zoomSizeSelected (const char* s) {
  float value;
  sscanf (s, "%3f", &value);
  if (canvas)
    canvas->setZoomFactor (value / 100.0);
}

void KIllustratorView::popupForSelection (int x, int y) {
  objMenu->popup (QCursor::pos ());
}

void KIllustratorView::toolMovePoint () {
  editPointTool->setMode (EditPointTool::MovePoint);
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_INSERT, false);
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_DELETE, false);
}

void KIllustratorView::toolInsertPoint () {
  editPointTool->setMode (EditPointTool::InsertPoint);
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_MOVE, false);
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_DELETE, false);
}

void KIllustratorView::toolRemovePoint () {
  editPointTool->setMode (EditPointTool::RemovePoint);
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_MOVE, false);
  m_vToolBarEditPoint->setButton (ID_TOOL_EP_INSERT, false);
}

void KIllustratorView::resetTools () {
  m_vToolBarTools->setButton (m_idActiveTool, false);
  m_vToolBarEditPoint->enable (OpenPartsUI::Hide);
  tcontroller->toolSelected (m_idActiveTool = ID_TOOL_SELECT);
  m_vToolBarTools->setButton (m_idActiveTool, true);
}

void KIllustratorView::insertPartSlot (KIllustratorChild *child, GPart *part) {
  cout << "INSERT PART SLOT =============================" << endl;
  OpenParts::View_var v = child->createView (m_vKoMainWindow);
  assert (! CORBA::is_nil (v));
  KIllustratorFrame *frame = new KIllustratorFrame (this, child);
  frame->setGeometry (child->geometry ());
  frame->setPartObject (part);
  m_lstFrames.append (frame);

  KOffice::View_var kv = KOffice::View::_narrow (v);
  kv->setMode (KOffice::View::ChildMode);
  assert (!CORBA::is_nil(kv));

  frame->attachView (kv);
  frame->hide ();
  cout << "set view for part: " << frame << endl;
  part->setView (frame);
  part->setMainWindow (mainWindow ());
  part->setParentID (id ());
  /*
  QObject::connect (frame, SIGNAL(sig_geometryEnd (KoFrame *)),
		    this, SLOT(childGeometryEndSlot (KoFrame *)));
  QObject::connect (frame, SIGNAL(sig_moveEnd (KoFrame *)),
		    this, SLOT(childMoveEndSlot (KoFrame *)));
  */
}

void KIllustratorView::childGeometryEndSlot (KoFrame *f) {
  KIllustratorFrame* frame = (KIllustratorFrame *) f;
  m_pDoc->changeChildGeometry (frame->child (), frame->partGeometry ());
}

void KIllustratorView::childMoveEndSlot (KoFrame *f) {
  KIllustratorFrame* frame = (KIllustratorFrame *) f;
  m_pDoc->changeChildGeometry (frame->child (), frame->partGeometry ());
}

void KIllustratorView::changeChildGeometrySlot (KIllustratorChild *child) {
  cout << "+++++++changeChildGeometry" << endl;
  // Find frame for child
  KIllustratorFrame *f = 0L;
  QListIterator<KIllustratorFrame> it (m_lstFrames);
  for (; it.current () && !f; ++it)
    if (it.current ()->child () == child)
      f = it.current ();

  assert(f != 0L);

  // Are we already up to date ?
  if (child->geometry () == f->partGeometry ())
    return;

  // TODO scaling
  f->setPartGeometry (child->geometry ());
}

void KIllustratorView::setFramesToParts () {
  KIllustratorFrame *frame = 0L;
  for (unsigned int i = 0; i < m_lstFrames.count (); i++) {
    frame = m_lstFrames.at (i);
    frame->hide ();
    frame->view ()->setMainWindow (mainWindow ());
    frame->getPartObject ()->setView (frame);
    frame->getPartObject ()->setMainWindow (mainWindow ());
    frame->getPartObject ()->setParentID (id ());
  }
}

void KIllustratorView::activatePart (GObject *obj) {
  if (obj->isA ("GPart")) {
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
  }
}

void KIllustratorView::showScripts () {
  if (!scriptDialog)
    scriptDialog = new ScriptDialog ();
  scriptDialog->setActiveDocument (m_pDoc);
  scriptDialog->show ();
  scriptDialog->loadScripts ();
}
