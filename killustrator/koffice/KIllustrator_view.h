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

#ifndef KIllustrator_view_h_
#define KIllustrator_view_h_

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include <qlist.h>
#include <qlayout.h>

#include "KIllustrator.h"
#include "KIllustrator_doc.h"

#include "MainView.h"
#include "CommandHistory.h"

#define ID_TOOL_SELECT        1001
#define ID_TOOL_EDITPOINT     1002
#define ID_TOOL_FREEHAND      1003
#define ID_TOOL_LINE          1004
#define ID_TOOL_BEZIER        1005
#define ID_TOOL_RECTANGLE     1006
#define ID_TOOL_POLYGON       1007
#define ID_TOOL_ELLIPSE       1008
#define ID_TOOL_TEXT          1009
#define ID_TOOL_ZOOM          1010
#define ID_TOOL_PATHTEXT      1011

class KIlustratorView;
class KIllustratorDocument;
class Canvas;
class GDocument;
class QwViewport;
class LayerDialog;
class ToolController;
class Canvas;
class Ruler;

class KIllustratorFrame : public KoFrame {
  Q_OBJECT
public:
  KIllustratorFrame (KIllustratorView* view, KIllustratorChild* child);
  KIllustratorChild* child () { return m_pChild; }
  KIllustratorView* killustratorView () { return m_pView; }

protected:
  KIllustratorChild *m_pChild;
  KIllustratorView *m_pView;
};

class KIllustratorView : public QWidget, public MainView,
			 virtual public KoViewIf,
			 virtual public KIllustrator::View_skel {
  Q_OBJECT
public:
  KIllustratorView (QWidget* parent, const char* name = 0L, 
		    KIllustratorDocument* doc = 0L);
  ~KIllustratorView ();

  void createGUI ();
  void construct ();
  void cleanUp ();

  GDocument* activeDocument () { return m_pDoc; }
  Canvas* getCanvas () { return canvas; }

  // --- IDL ---
  void newView ();
  //  void setMode (OPParts::Part::Mode mode);
  //  void setFocus (CORBA::Boolean mode);
  CORBA::Boolean printDlg ();

  void editUndo ();
  void editRedo ();
  void editCut ();
  void editCopy ();
  void editPaste ();
  void editSelectAll ();
  void editDelete ();
  void editInsertOject ();
  void editProperties ();

  void transformPosition ();
  void transformDimension ();
  void transformRotation ();
  void transformMirror ();

  void arrangeAlign ();
  void arrangeToFront ();
  void arrangeToBack ();
  void arrangeOneForward ();
  void arrangeOneBack ();
  void arrangeGroup ();
  void arrangeUngroup ();

  void toggleRuler ();
  void toggleGrid ();

  void setupGrid ();
  void alignToGrid ();

  void editLayers ();

  void setPenColor (CORBA::Long id);
  void setFillColor (CORBA::Long id);

  void toolSelection ();
  void toolEditPoint ();
  void toolFreehandLine ();
  void toolPolyline ();
  void toolBezier ();
  void toolRectangle ();
  void toolPolygon ();
  void toolEllipse ();
  void toolText ();
  void toolZoom ();

  void configPolygonTool ();

protected:
  void init ();
  bool event (const char* _event, const CORBA::Any& _value);
  bool mappingCreateMenubar (OpenPartsUI::MenuBar_ptr menubar);
  bool mappingCreateToolbar (OpenPartsUI::ToolBarFactory_ptr factory);

  void showTransformationDialog (int id);

  void setupColorToolbar ();
  void setupCanvas ();
  void resizeEvent (QResizeEvent*);

protected slots:
  void setUndoStatus(bool undoPossible, bool redoPossible);
  void showCurrentMode (const char* msg);

protected:
  /* Menu: Edit */
  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_Undo, m_idMenuEdit_Redo,
    m_idMenuEdit_Cut, m_idMenuEdit_Copy, m_idMenuEdit_Paste, 
    m_idMenuEdit_Delete, m_idMenuEdit_SelectAll, 
    m_idMenuEdit_InsertObject, m_idMenuEdit_Properties;
  /* Menu: View */
  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_Outline, 
    m_idMenuView_Normal, m_idMenuView_Ruler, m_idMenuView_Grid;
  /* Menu: Layout */
  OpenPartsUI::Menu_var m_vMenuLayout;
  CORBA::Long m_idMenuLayout_InsertPage, 
    m_idMenuLayout_RemovePage, m_idMenuLayout_GotoPage, 
    m_idMenuLayout_PageLayout, m_idMenuLayout_Layers, 
    m_idMenuLayout_SetupGrid, m_idMenuLayout_AlignToGrid;
  /* Menu: Arrange */
  OpenPartsUI::Menu_var m_vMenuArrange;
  OpenPartsUI::Menu_var m_vMenuTransform;
  CORBA::Long m_idMenuTransform_Position,
    m_idMenuTransform_Dimension, m_idMenuTransform_Rotation, 
    m_idMenuTransform_Mirror, m_idMenuArrange_Align, 
    m_idMenuArrange_ToFront, m_idMenuArrange_ToBack, 
    m_idMenuArrange_1Forward, m_idMenuArrange_1Back, 
    m_idMenuArrange_Group, m_idMenuArrange_Ungroup;
  /* Menu: Extras */
  OpenPartsUI::Menu_var m_vMenuExtras;
  /* Menu: Help */
  OpenPartsUI::Menu_var m_vMenuHelp;

  /* Toolbar: Tools */
  OpenPartsUI::ToolBar_var m_vToolBarTools;
  CORBA::Long m_idSelectionTool;
  CORBA::Long m_idEditPointTool;
  CORBA::Long m_idFreeHandTool;
  CORBA::Long m_idPolylineTool;
  CORBA::Long m_idBezierTool;
  CORBA::Long m_idRectangleTool;
  CORBA::Long m_idPolygonTool;
  CORBA::Long m_idEllipseTool;
  CORBA::Long m_idTextTool;
  CORBA::Long m_idZoomTool;
  CORBA::Long m_idActiveTool;

  /* Toolbar: Colors */
  OpenPartsUI::ToolBar_var m_vToolBarColors;
  CORBA::Long m_idFirstColor;
  QVector<QColor> colorPalette;

  KIllustratorDocument *m_pDoc;

  QList<KIllustratorFrame> m_lstFrames;
  QArray<float> zFactors;

  bool m_bShowGUI;
  bool m_bShowRulers;

  ToolController *tcontroller;
  QwViewport *viewport;
  Canvas *canvas;
  Ruler *hRuler, *vRuler;
  LayerDialog *layerDialog;
  QWidget *mainWidget;
  QGridLayout *grid;
  CommandHistory cmdHistory;
};

#endif
