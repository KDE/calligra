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

#include <part_frame_impl.h>
#include <view_impl.h>
#include <document_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>

#include <qlist.h>
#include <qlayout.h>

#include "KIllustrator.h"
#include "KIllustrator_doc.h"

#include "MainView.h"
#include "CommandHistory.h"

class KIllustratorDocument;
class Canvas;
class GDocument;
class QwViewport;
class TransformationDialog;
class ToolController;
class Canvas;
class Ruler;

class KIllustratorChildFrame : public PartFrame_impl {
  Q_OBJECT
public:
  KIllustratorChildFrame (KIllustratorView* view, KIllustratorChild* child);
  KIllustratorChild* child () { return m_pChild; }
  KIllustratorView* view () { return m_pView; }

protected:
  KIllustratorChild *m_pChild;
  KIllustratorView *m_pView;
};

class KIllustratorView : public QWidget, public MainView,
			 virtual public View_impl,
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
  void setMode (OPParts::Part::Mode mode);
  void setFocus (CORBA::Boolean mode);
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

  void activateSelectionTool ();
  void activateEditPointTool ();
  void activateLineTool ();
  void activateBezierTool ();
  void activateRectangleTool ();
  void activatePolygonTool ();
  void activateEllipseTool ();
  void activateTextTool ();
  void activateZoomTool ();

protected:
  CORBA::Long addToolButton (const char* pictname, const char* tooltip,
			     const char* func);
  void showTransformationDialog (int id);

  void setupMenu ();
  void setupColorToolbar ();
  void setupMainToolbar ();
  void setupToolsToolbar ();
  void setupCanvas ();
  void resizeEvent (QResizeEvent*);

protected slots:
  void setUndoStatus(bool undoPossible, bool redoPossible);

protected:

  OPParts::MenuBarFactory_var m_vMenuBarFactory;
  OPParts::ToolBarFactory_var m_vToolBarFactory;

  MenuBar_ref m_rMenuBar;
  /* Menu: Edit */
  CORBA::Long m_idMenuEdit, m_idMenuEdit_Undo, m_idMenuEdit_Redo,
    m_idMenuEdit_Cut, m_idMenuEdit_Copy, m_idMenuEdit_Paste, 
    m_idMenuEdit_Delete, m_idMenuEdit_SelectAll, 
    m_idMenuEdit_InsertObject, m_idMenuEdit_Properties;
  /* Menu: View */
  CORBA::Long m_idMenuView, m_idMenuView_Outline, m_idMenuView_Draft,
    m_idMenuView_Normal, m_idMenuView_Ruler, m_idMenuView_Grid;
  /* Menu: Layout */
  CORBA::Long m_idMenuLayout, m_idMenuLayout_InsertPage, 
    m_idMenuLayout_RemovePage, m_idMenuLayout_GotoPage, 
    m_idMenuLayout_PageLayout, m_idMenuLayout_Layers, 
    m_idMenuLayout_SetupGrid, m_idMenuLayout_AlignToGrid;
  /* Menu: Arrange */
  CORBA::Long m_idMenuArrange, m_idMenuTransform, m_idMenuTransform_Position,
    m_idMenuTransform_Dimension, m_idMenuTransform_Rotation, 
    m_idMenuTransform_Mirror, m_idMenuArrange_Align, 
    m_idMenuArrange_ToFront, m_idMenuArrange_ToBack, 
    m_idMenuArrange_1Forward, m_idMenuArrange_1Back, 
    m_idMenuArrange_Group, m_idMenuArrange_Ungroup;
  /* Menu: Extras */
  CORBA::Long m_idMenuExtras;
  /* Menu: Help */
  CORBA::Long m_idMenuHelp_About;

  /* Toolbar: Tools */
  ToolBar_ref m_rToolBarTools;
  CORBA::Long m_idSelectionTool;
  CORBA::Long m_idEditPointTool;
  CORBA::Long m_idPolylineTool;
  CORBA::Long m_idBezierTool;
  CORBA::Long m_idRectangleTool;
  CORBA::Long m_idPolygonTool;
  CORBA::Long m_idEllipseTool;
  CORBA::Long m_idTextTool;
  CORBA::Long m_idZoomTool;
  CORBA::Long m_idActiveTool;

  Part_impl *m_pPart;
  KIllustratorDocument *m_pDoc;

  QList<KIllustratorChildFrame> m_lstFrames;

  bool m_bShowGUI;
  bool m_bShowRulers;

  ToolController *tcontroller;
  QwViewport *viewport;
  Canvas *canvas;
  Ruler *hRuler, *vRuler;
  TransformationDialog *transformationDialog;
  QWidget *mainWidget;
  QGridLayout *grid;
  CommandHistory cmdHistory;
  static QList<GObject> clipboard;
};

#endif
