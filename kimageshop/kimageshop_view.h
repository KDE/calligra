/*
 *  kimageshop_view.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <koch@kde.org>
 *                1999 Carsten Pfeiffer <pfeiffer@kde.org>
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

#include <container.h>
#include "kfloatingdialogowner.h"

class KAction;
class KToggleAction;
class QPaintEvent;

class BrushDialog;
class GradientDialog;
class GradientEditorDialog;
class ColorDialog;
class LayerDialog;

class KImageShopDoc;
class QScrollBar;
class KRuler;

class Brush;
class BrushesWidget;
class Tool;
class MoveTool;
class BrushTool;
class ZoomTool;
class GradientTool;

class KImageShopView : public ContainerView
{
  Q_OBJECT;

 public:
  KImageShopView( KImageShopDoc* doc, QWidget* parent = 0, const char* name = 0 );

 public slots:
  void slotDocUpdated();
  void slotDocUpdated(const QRect&);
  void slotSetBrush(const Brush *);

 signals:
  void mousePressed(QMouseEvent *);
  void mouseMoved(QMouseEvent *);
  void mouseReleased(QMouseEvent *);

 protected slots:
  // edit action slots
  void undo();
  void redo();
  void copy();
  void cut();
  void paste();

  // dialog action slots
  void dialog_layer();
  void dialog_color();
  void dialog_brush();
  void dialog_gradient();
  void dialog_gradienteditor();

  // layer action slots
  void layer_rotate180();
  void layer_rotateleft90();
  void layer_rotateright90();
  void layer_mirrorX();
  void layer_mirrorY();

  // image action slots
  void merge_all_layers();
  void merge_visible_layers();
  void merge_linked_layers();
  
  // tool action slots
  void tool_move();
  void tool_zoom();
  void tool_brush();
  void tool_gradient();

  // misc action slots
  void preferences();

  // scrollbar slots
  void scrollH(int);
  void scrollV(int);


 protected:
  virtual void paintEvent( QPaintEvent* );
  virtual void resizeEvent( QResizeEvent* );
  virtual void mousePressEvent ( QMouseEvent * );
  virtual void mouseReleaseEvent ( QMouseEvent * );
  virtual void mouseMoveEvent ( QMouseEvent * );

  void setupScrollBars();
  void setupRulers();

  void activateTool(Tool*);

 private:
  int 	docWidth();
  int 	docHeight();

  int 	xPaintOffset();
  int 	yPaintOffset();

  // edit actions
  KAction *m_undo, *m_redo, *m_copy, *m_cut, *m_paste;
  // dialog actions
  KToggleAction *m_dialog_layer, *m_dialog_color, *m_dialog_brush, *m_dialog_gradient, *m_dialog_gradienteditor;
  // tool actions
  KToggleAction *m_tool_move, *m_tool_zoom, *m_tool_brush, *m_tool_gradient;
  // layer actions
  KAction *m_layer_rotate180, *m_layer_rotateleft90, *m_layer_rotateright90, *m_layer_mirrorX, *m_layer_mirrorY;
  // misc actions
  KAction *m_preferences;
  // image actions
  KAction *m_merge_all_layers, *m_merge_visible_layers, *m_merge_linked_layers;

  KImageShopDoc        *m_pDoc;
  Tool                 *m_pTool; // currently active tool
  MoveTool             *m_pMoveTool;
  BrushTool            *m_pBrushTool;
  ZoomTool             *m_pZoomTool;
  GradientTool         *m_pGradientTool;
  const Brush          *m_pBrush; // current brush
  const BrushesWidget  *m_pBrushChooser;

  BrushDialog          *m_pBrushDialog;
  LayerDialog          *m_pLayerDialog;
  GradientDialog       *m_pGradientDialog;
  GradientEditorDialog *m_pGradientEditorDialog;
  ColorDialog          *m_pColorDialog;

  QScrollBar           *m_pHorz, *m_pVert;
  KRuler               *m_pHRuler, *m_pVRuler;
};

#endif
