/*
 *  kimageshop_view.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
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

#include <qwidget.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include <koFrame.h>
#include <koView.h>

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
class ColorDialog;

class KImageShopView : public QWidget,
		       virtual public KoViewIf,
		       virtual public KImageShop::View_skel
{
  Q_OBJECT
    
public:

  KImageShopView(QWidget* _parent, const char* _name, KImageShopDoc* _doc);
  ~KImageShopView();
  
  virtual void cleanUp();
  virtual void createGUI();

  float zoomFactor() { return m_ZoomFactor; }

  KImageShopDoc* doc();
  CORBA::Boolean printDlg();

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

  virtual void slotEditUndo();
  virtual void slotEditUndo2();
  virtual void slotEditUndo3();
  virtual void slotEditUndo4();
  virtual void slotEditUndo5();
  virtual void slotEditUndo6();
  virtual void slotEditUndo7();
  virtual void slotEditUndo8();
  virtual void slotEditUndo9();
  virtual void slotEditUndo10();
  virtual void slotEditRedo();
  virtual void slotEditRedo2();
  virtual void slotEditRedo3();
  virtual void slotEditRedo4();
  virtual void slotEditRedo5();
  virtual void slotEditRedo6();
  virtual void slotEditRedo7();
  virtual void slotEditRedo8();
  virtual void slotEditRedo9();
  virtual void slotEditRedo10();

  virtual void slotEditCut();
  virtual void slotEditCopy();
  virtual void slotEditPaste();

  virtual void viewLayerDialog();
  virtual void viewColorDialog();
  virtual void viewPreferences();

  virtual void slotActivateMoveTool();
  virtual void slotActivateBrushTool();
  virtual void slotActivateZoomTool();

  void changeUndo( QString, bool);
  void changeRedo( QString, bool);

protected:

  virtual void init();

  void setupScrollbars();
  void setupRulers();

  virtual bool event( const char* _event, const CORBA::Any& _value );
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

  virtual void newView();
  virtual void helpUsing();

  virtual void resizeEvent(QResizeEvent* _ev);

  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  OpenPartsUI::ToolBar_var m_vToolBarTools;

  OpenPartsUI::Menu_var m_vTBUndoMenu;
  OpenPartsUI::Menu_var m_vTBRedoMenu;

  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_Undo;
  CORBA::Long m_idMenuEdit_Redo;
  CORBA::Long m_idMenuEdit_Cut;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_Paste;

  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_LayerDialog;
  CORBA::Long m_idMenuView_ColorDialog;
  CORBA::Long m_idMenuView_Preferences;

  OpenPartsUI::Menu_var m_vMenuImage;
  OpenPartsUI::Menu_var m_vMenuPlugIns;
  OpenPartsUI::Menu_var m_vMenuOptions;
  
private:

  enum { TBTOOLS_MOVETOOL, TBTOOLS_BRUSHTOOL, TBTOOLS_ZOOMTOOL, TBEDIT_UNDO, TBEDIT_REDO,
	 TBEDIT_COPY, TBEDIT_CUT, TBEDIT_PASTE };

  KImageShopDoc *m_pDoc; 
  LayerDialog   *m_pLayerDialog;
  QScrollBar    *m_pHorz, *m_pVert;
  KRuler        *m_pHRuler, *m_pVRuler;

  CanvasView    *m_pCanvasView;
  Tool          *m_pTool; // currently active tool
  MoveTool      *m_pMoveTool;
  BrushTool     *m_pBrushTool;
  ZoomTool      *m_pZoomTool;
  Brush         *m_pBrush; // current brush
  ColorDialog   *m_pColorDialog;
  float         m_ZoomFactor;
};

#endif
