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

#include <qarray.h>

#include "CommandHistory.h"
#include "MainView.h"

#include <koView.h>

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

#define ID_TOOL_INSERTPART    1012

#define ID_EDIT_UNDO          2001
#define ID_EDIT_REDO          2002
#define ID_EDIT_CUT           2003
#define ID_EDIT_COPY          2004
#define ID_EDIT_PASTE         2005
#define ID_EDIT_DELETE        2006
#define ID_EDIT_ZOOM          2007

#define ID_TOOL_EP_MOVE       1101
#define ID_TOOL_EP_INSERT     1102
#define ID_TOOL_EP_DELETE     1103
#define ID_TOOL_EP_SPLIT      1104
#define ID_TOOL_EP_JOIN       1105

class KIllustratorView;
class KIllustratorChild;
class KIllustratorDocument;
class Canvas;
class GDocument;
class QwViewport;
class LayerDialog;
class ToolController;
class Canvas;
class Ruler;
class EditPointTool;
class InsertPartTool;
class GPart;
class QGridLayout;
class FilterManager;

class KAction;
class KToggleAction;
class KColorBarAction;
class KSelectAction;

class KIllustratorView : public KoView, public MainView
{
    Q_OBJECT
public:
    KIllustratorView (QWidget* parent, const char* name = 0, KIllustratorDocument* doc = 0 );
    ~KIllustratorView ();

    void createMyGUI ();

    /**
     * Overloaded @ref MainView::activeDocument
     */
    GDocument* activeDocument ();
    /**
     * Overloaded @ref MainView::getCanvas
     */
    Canvas* getCanvas () { return canvas; }

    bool printDlg();

    /* void newView ();
  bool printDlg ();

  void editUndo ();
  void editRedo ();
  void editCut ();
  void editCopy ();
  void editPaste ();
  void editSelectAll ();
  void editDelete ();
  void editDuplicate ();
  void editProperties ();

  void editInsertObject ();
  void editInsertClipart ();
  void editInsertBitmap ();

  void viewOutline ();
  void viewNormal ();

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
  void arrangeTextAlongPath ();

  void toggleRuler ();
  void toggleGrid ();
  void toggleHelplines ();

  void setupGrid ();
  void alignToGrid ();
  void setupHelplines ();
  void alignToHelplines ();

  void editLayers ();
  void setupPage ();

  void setPenColor (long int id);
  void setFillColor (long int id);

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

  void toolMovePoint ();
  void toolInsertPoint ();
  void toolRemovePoint ();

  void configPolygonTool ();
  void configEllipseTool ();

  void zoomSizeSelected (const QString & s);

  // void showScripts (); */

protected:
    void showTransformationDialog (int id);

    void setupCanvas ();
    void setupPopups ();
    void resizeEvent (QResizeEvent*);

    virtual void updateReadWrite( bool readwrite );

protected slots:
    void setUndoStatus( bool undoPossible, bool redoPossible );
    void popupForSelection (int x, int y);
    void resetTools();
    QString getExportFileName (FilterManager *filterMgr);

  void showCurrentMode (const char* msg);
    /* protected slots:
  void editCutSlot ();
  void editCopySlot ();
  void editPropertiesSlot ();
  void arrangeAlignSlot ();
  void arrangeToFrontSlot ();
  void arrangeToBackSlot ();
  void arrangeOneForwardSlot ();
  void arrangeOneBackSlot ();

  void popupForSelection (int x, int y);
  void setUndoStatus(bool undoPossible, bool redoPossible);
  void resetTools ();

  void insertPartSlot (KIllustratorChild *child, GPart *part);
  void changeChildGeometrySlot (KIllustratorChild *child); */

private slots:
    /**
     * Actions
     */
    void slotImport();
    void slotExport();
    void slotInsertBitmap();
    void slotInsertClipart();
    void slotCopy();
    void slotPaste();
    void slotCut();
    void slotUndo();
    void slotRedo();
    void slotDuplicate();
    void slotDelete();
    void slotSelectAll();
    void slotProperties();
    void slotOutline( bool );
    void slotNormal( bool );
    void slotShowRuler( bool );
    void slotShowGrid( bool );
    void slotShowHelplines( bool );
    void slotPage();
    void slotGrid();
    void slotHelplines();
    void slotAlignToGrid( bool );
    void slotAlignToHelplines( bool );
    void slotTransformPosition();
    void slotTransformDimension();
    void slotTransformRotation();
    void slotTransformMirror();
    void slotDistribute();
    void slotToFront();
    void slotToBack();
    void slotForwardOne();
    void slotBackOne();
    void slotGroup();
    void slotUngroup();
    void slotTextAlongPath();
    void slotConvertToCurve();
    void slotBlend();
    void slotOptions();
    void slotBrushChosen( const QColor & );
    void slotPenChosen( const QColor & );
    void slotSelectTool( bool );
    void slotPointTool( bool );
    void slotFreehandTool( bool );
    void slotLineTool( bool );
    void slotBezierTool( bool );
    void slotRectTool( bool );
    void slotPolygonTool( bool );
    void slotEllipseTool( bool );
    void slotTextTool( bool );
    void slotZoomTool( bool );
    void slotMoveNode( bool );
    void slotNewNode( bool );
    void slotDeleteNode( bool );
    void slotSplitLine( bool );
    void slotLayers();
  void slotViewZoom (const QString&);
  void slotLoadPalette ();

protected:
  KIllustratorDocument *m_pDoc;
  EditPointTool *editPointTool;
  InsertPartTool *insertPartTool;
  QPopupMenu *objMenu;

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

    // int m_idActiveTool;

    KAction* m_import;
    KAction* m_export;
    KAction* m_insertBitmap;
    KAction* m_insertClipart;
    KAction* m_copy;
    KAction* m_paste;
    KAction* m_cut;
    KAction* m_undo;
    KAction* m_redo;
    KAction* m_duplicate;
    KAction* m_delete;
    KAction* m_selectAll;
    KAction* m_properties;
    KToggleAction* m_outline;
    KToggleAction* m_normal;
    KAction* m_layers;
    KToggleAction* m_showRuler;
    KToggleAction* m_showGrid;
    KToggleAction* m_showHelplines;
    KAction* m_page;
    KAction* m_grid;
    KAction* m_helplines;
    KToggleAction* m_alignToGrid;
    KToggleAction* m_alignToHelplines;
    KAction* m_transformPosition;
    KAction* m_transformDimension;
    KAction* m_transformRotation;
    KAction* m_transformMirror;
    KAction* m_distribute;
    KAction* m_toFront;
    KAction* m_toBack;
    KAction* m_forwardOne;
    KAction* m_backOne;
    KAction* m_group;
    KAction* m_ungroup;
    KAction* m_textAlongPath;
    KAction* m_convertToCurve;
    KAction* m_blend;
    KAction* m_options;
    KAction* m_loadPalette;
    KColorBarAction* m_colorBar;
    KToggleAction* m_selectTool;
    KToggleAction* m_pointTool;
    KToggleAction* m_freehandTool;
    KToggleAction* m_lineTool;
    KToggleAction* m_bezierTool;
    KToggleAction* m_rectTool;
    KToggleAction* m_polygonTool;
    KToggleAction* m_ellipseTool;
    KToggleAction* m_textTool;
    KToggleAction* m_zoomTool;
    KToggleAction* m_moveNode;
    KToggleAction* m_newNode;
    KToggleAction* m_deleteNode;
    KToggleAction* m_splitLine;
  KSelectAction* m_viewZoom;
    QString lastOpenDir, lastSaveDir, lastBitmapDir, lastClipartDir,
            lastExportDir, lastImportDir, lastPaletteDir;
    QString lastExport;
};

#endif
