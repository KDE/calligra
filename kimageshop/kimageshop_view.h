/*
 *  kimageshop_view.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <koch@kde.org>
 *                1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kimageshop_view_h__
#define __kimageshop_view_h__

#include <kcolor.h>
#include <qwidget.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include <koFrame.h>
#include <koView.h>

#include "kfloatingdialogowner.h"
#include "kimageshop.h"

class CanvasView;
class LayerDialog;
class KImageShopDoc;
class QScrollBar;
class KRuler;
class Tool;
class MoveTool;
class BrushTool;
class ZoomTool;
class Brush;
class Gradient;
class GradientTool;
class GradientDialog;
class GradientEditorDialog;
class ColorDialog;
class BrushDialog;
class BrushesWidget;

class KImageShopView : public QWidget,
		       virtual public KoViewIf,
		       virtual public KImageShop::View_skel,
		       virtual public KFloatingDialogOwner
{
  Q_OBJECT

public:

  KImageShopView(QWidget* _parent, const char* _name, KImageShopDoc* _doc);
  ~KImageShopView();

  virtual void cleanUp();
  virtual void createGUI();

  float zoomFactor() { return m_ZoomFactor; }

  KImageShopDoc* doc();
  bool printDlg();

  int xPaintOffset();
  int yPaintOffset();

  int docWidth();
  int docHeight();

  int viewWidth();
  int viewHeight();

  int hScrollValue();
  int vScrollValue();

  void scrollTo(const QPoint & pos);

public slots:

  void slotUpdateView(const QRect &area);

  void slotCVPaint(QPaintEvent *e);
  void slotCVMousePress(QMouseEvent *e);
  void slotCVMouseMove(QMouseEvent *e);
  void slotCVMouseRelease(QMouseEvent *e);

  void scrollH(int);
  void scrollV(int);

  void slotSetZoomFactor(float);
  void slotSetFGColor(const KColor&);
  void slotSetBGColor(const KColor&);
  void slotSetBrush(const Brush *);
  
  virtual void slotEditUndo();
  virtual void slotEditRedo();

  virtual void slotEditCut();
  virtual void slotEditCopy();
  virtual void slotEditPaste();

  virtual void slotLayerDialog();
  virtual void slotColorDialog();
  virtual void slotBrushDialog();
  virtual void slotGradientDialog();
  virtual void slotGradientEditorDialog();
  virtual void slotPreferences();

  virtual void slotActivateMoveTool();
  virtual void slotActivateBrushTool();
  virtual void slotActivateZoomTool();
  virtual void slotActivateGradientTool();

	virtual void slotRotateLayer180();

  void changeUndo( QString, bool);
  void changeRedo( QString, bool);

  void activatedUndoMenu( long );
  void activatedRedoMenu( long );

private:

  void undo( int _number );
  void redo( int _number );

protected:

  virtual void init();

  void setupScrollbars();
  void setupRulers();

  virtual bool event( const QCString &_event, const CORBA::Any& _value );
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

  virtual void newView();
  virtual void helpUsing();

  virtual void resizeEvent(QResizeEvent* _ev);

  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  OpenPartsUI::ToolBar_var m_vToolBarTools;
  OpenPartsUI::ToolBar_var m_vToolBarDialogs;

public:
  OpenPartsUI::Menu_var m_vTBUndoMenu;
  OpenPartsUI::Menu_var m_vTBRedoMenu;

protected:
  OpenPartsUI::Menu_var m_vMenuEdit;
  long m_idMenuEdit_Undo;
  long m_idMenuEdit_Redo;
  long m_idMenuEdit_Cut;
  long m_idMenuEdit_Copy;
  long m_idMenuEdit_Paste;

  OpenPartsUI::Menu_var m_vMenuView;
  long m_idMenuView_LayerDialog;
  long m_idMenuView_ColorDialog;
  long m_idMenuView_BrushDialog;
  long m_idMenuView_GradientDialog;
  long m_idMenuView_GradientEditorDialog;
  long m_idMenuView_Preferences;

  OpenPartsUI::Menu_var m_vMenuImage;
	long m_idMenuImage_Rotate180;

  OpenPartsUI::Menu_var m_vMenuPlugIns;
  OpenPartsUI::Menu_var m_vMenuOptions;

private:

  enum { TBTOOLS_MOVETOOL, TBTOOLS_BRUSHTOOL, TBTOOLS_ZOOMTOOL, TBTOOLS_GRADIENTTOOL, TBEDIT_UNDO,
	 TBEDIT_REDO, TBEDIT_COPY, TBEDIT_CUT, TBEDIT_PASTE, TBDIALOGS_LAYER, TBDIALOGS_COLOR,
	 TBDIALOGS_BRUSH, TBDIALOGS_GRADIENT, TBDIALOGS_GRADIENTEDITOR };

  KImageShopDoc        *m_pDoc;
  LayerDialog          *m_pLayerDialog;
  QScrollBar           *m_pHorz, *m_pVert;
  KRuler               *m_pHRuler, *m_pVRuler;

  CanvasView           *m_pCanvasView;
  Tool                 *m_pTool; // currently active tool
  MoveTool             *m_pMoveTool;
  BrushTool            *m_pBrushTool;
  ZoomTool             *m_pZoomTool;
  const Brush          *m_pBrush; // current brush
  BrushDialog          *m_pBrushDialog;
  const BrushesWidget  *m_pBrushChooser;
  Gradient             *m_actGradient;
  GradientTool         *m_pGradientTool;
  GradientDialog       *m_pGradientDialog;
  GradientEditorDialog *m_pGradientEditorDialog;
  ColorDialog          *m_pColorDialog;
  KColor               m_fg, m_bg;
  float                m_ZoomFactor;

  vector<int>          m_toolButtons; // id's from all Tools
  void activateTool( int toolID );
};

#endif








