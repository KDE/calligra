/*
 *  kis_view.h - part of KImageShop
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

#ifndef __kis_view_h__
#define __kis_view_h__

#include <container.h>

#include "kfloatingdialogowner.h"
#include "kis_color.h"

class KAction;
class KToggleAction;
class QPaintEvent;

class BrushDialog;
class GradientDialog;
class GradientEditorDialog;
class ColorDialog;
class LayerDialog;

class kisDoc;
class kisCanvas;
class kisTabBar;
class QScrollBar;
class KRuler;

class Brush;
class BrushesWidget;
class Tool;
class MoveTool;
class BrushTool;
class AirBrushTool;
class EraserTool;
class PenTool;
class ZoomTool;
class GradientTool;
class ColorPicker;
class QButton;

class kisView : public ContainerView
{
  Q_OBJECT;

 public:
  kisView( kisDoc* doc, QWidget* parent = 0, const char* name = 0 );

  KColor& fgColor() { return m_fg; }
  KColor& bgColor() { return m_bg; }

 public slots:
  void slotDocUpdated();
  void slotDocUpdated(const QRect&);
  void slotSetBrush(const Brush *);

  void slotSetFGColor(const KColor&);
  void slotSetBGColor(const KColor&);
  void slotScrollToFirstTab();
  void slotScrollToLeftTab();
  void slotScrollToRightTab();
  void slotScrollToLastTab();

  void slotTabSelected(const QString& name);
  void slotImageAdded(const QString& name);

 signals:
  void canvasMousePressEvent( QMouseEvent * );
  void canvasMouseMoveEvent( QMouseEvent * );
  void canvasMouseReleaseEvent( QMouseEvent * );

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
  void updateToolbarButtons();

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
  void tool_airbrush();
  void tool_pen();
  void tool_eraser();
  void tool_gradient();
  void tool_colorpicker();

  // misc action slots
  void preferences();

  // scrollbar slots
  void scrollH(int);
  void scrollV(int);

  void canvasGotMousePressEvent( QMouseEvent * );
  void canvasGotMouseMoveEvent ( QMouseEvent * );
  void canvasGotMouseReleaseEvent ( QMouseEvent * );
  void canvasGotPaintEvent( QPaintEvent* );

 protected:
  virtual void resizeEvent( QResizeEvent* );

  void setupCanvas();
  void setupScrollBars();
  void setupRulers();
  void setupTools();
  void setupDialogs();
  void setupActions();
  void setupTabBar();

  void activateTool(Tool*);

 private:
  int 	docWidth();
  int 	docHeight();

  int 	xPaintOffset();
  int 	yPaintOffset();
  float zoomFactor();

  // edit actions
  KAction *m_undo, *m_redo, *m_copy, *m_cut, *m_paste;
  // dialog actions
  KToggleAction *m_dialog_layer, *m_dialog_color, *m_dialog_brush, *m_dialog_gradient, *m_dialog_gradienteditor;
  // tool actions
  KToggleAction *m_tool_move, *m_tool_zoom, *m_tool_brush, *m_tool_pen
    , *m_tool_gradient, *m_tool_colorpicker, *m_tool_airbrush, *m_tool_eraser;
  // layer actions
  KAction *m_layer_rotate180, *m_layer_rotateleft90, *m_layer_rotateright90, *m_layer_mirrorX, *m_layer_mirrorY;
  // misc actions
  KAction *m_preferences;
  // image actions
  KAction *m_merge_all_layers, *m_merge_visible_layers, *m_merge_linked_layers;

  kisDoc               *m_pDoc;
  Tool                 *m_pTool; // currently active tool
  MoveTool             *m_pMoveTool;
  BrushTool            *m_pBrushTool;
  EraserTool           *m_pEraserTool;
  AirBrushTool         *m_pAirBrushTool;
  PenTool              *m_pPenTool;
  ZoomTool             *m_pZoomTool;
  GradientTool         *m_pGradientTool;
  ColorPicker          *m_pColorPicker;
  const Brush          *m_pBrush; // current brush
  const BrushesWidget  *m_pBrushChooser;

  BrushDialog          *m_pBrushDialog;
  LayerDialog          *m_pLayerDialog;
  GradientDialog       *m_pGradientDialog;
  GradientEditorDialog *m_pGradientEditorDialog;
  ColorDialog          *m_pColorDialog;

  kisCanvas            *m_pCanvas;
  QScrollBar           *m_pHorz, *m_pVert;
  KRuler               *m_pHRuler, *m_pVRuler;
  KColor                m_fg, m_bg;

  kisTabBar            *m_pTabBar;
  QButton              *m_pTabFirst, *m_pTabLeft, *m_pTabRight, *m_pTabLast;
};

#endif
