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

#include <koView.h>

#include "kis_color.h"

class KAction;
class KToggleAction;
class KHelpMenu;
class QPaintEvent;

class GradientDialog;
class GradientEditorDialog;

class KisDoc;
class KisCanvas;
class KisTabBar;
class KisSideBar;

class KisBrushChooser;
class KisLayerView;
class KisChannelView;

class QScrollBar;
class KRuler;

class SelectTool;
class KisBrush;
class KisTool;
class MoveTool;
class BrushTool;
class AirBrushTool;
class EraserTool;
class PenTool;
class ZoomTool;
class KisGradient;
class GradientTool;
class ColorPicker;
class QButton;


class KisView : public KoView
{
    Q_OBJECT

 public:        // constructors
 
    KisView( KisDoc* doc, QWidget* parent = 0, const char* name = 0 );

    KisColor& fgColor() { return m_fg; }
    KisColor& bgColor() { return m_bg; }

    KisCanvas* kisCanvas() { return m_pCanvas; }
    void updateCanvas(  QRect & ur );
    
 public slots:
 
    void slotDocUpdated();
    void slotDocUpdated(const QRect&);
    void slotSetBrush(const KisBrush *);

    void slotSetFGColor(const KisColor&);
    void slotSetBGColor(const KisColor&);

    void slotTabSelected(const QString& name);

 signals:
 
    void canvasMousePressEvent( QMouseEvent * );
    void canvasMouseMoveEvent( QMouseEvent * );
    void canvasMouseReleaseEvent( QMouseEvent * );

 public slots:

    void zoom_in();
    void zoom_out();

 protected slots:
 
    // edit action slots
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();

    // dialog action slots
    void dialog_gradient();
    void dialog_gradienteditor();
    void updateToolbarButtons();

    // layer action slots
    void insert_layer();
    void insert_layer_image();
    void save_layer_image();
    void layer_rotate180();
    void layer_rotateleft90();
    void layer_rotateright90();
    void layer_mirrorX();
    void layer_mirrorY();

    // image action slots
    void add_new_image_tab();
    void remove_current_image_tab();
    void merge_all_layers();
    void merge_visible_layers();
    void merge_linked_layers();

    // tool action slots
    void tool_select_rect();
    void tool_move();
    void tool_zoom();
    void tool_brush();
    void tool_airbrush();
    void tool_pen();
    void tool_eraser();
    void tool_gradient();
    void tool_colorpicker();

    // settings action slots
    void showMenubar();
    void showToolbar();
    void showStatusbar();
    void showSidebar();
    void saveOptions();
    void preferences();

    // scrollbar slots
    void scrollH(int);
    void scrollV(int);

    void canvasGotMousePressEvent( QMouseEvent * );
    void canvasGotMouseMoveEvent ( QMouseEvent * );
    void canvasGotMouseReleaseEvent ( QMouseEvent * );
    void canvasGotPaintEvent( QPaintEvent* );

    void slotUndoRedoChanged( QString _undo, QString _redo );
    void slotUndoRedoChanged( QStringList _undo, QStringList _redo );

 protected:
 
    virtual void resizeEvent( QResizeEvent* );
    virtual void updateReadWrite( bool readwrite );

    void setupCanvas();
    void setupSideBar();
    void setupScrollBars();
    void setupRulers();
    void setupTools();
    void setupDialogs();
    void setupActions();
    void setupTabBar();

    void activateTool(KisTool*);

 public:

    int 	docWidth();
    int 	docHeight();

    int 	xPaintOffset();
    int 	yPaintOffset();
    void        scrollTo( QPoint p );

    void 	zoom( int x, int y, float zf );
    void        zoom_in( int x, int y );
    void	zoom_out( int x, int y );
    float       zoomFactor();
    void        setZoomFactor( float zf );
    
 private:

    // import/export actions
    KAction *m_layer_rotate180, *m_layer_rotate270, *m_layer_rotate90,
    *m_layer_mirrorX, *m_layer_mirrorY; 
        
    // edit actions
    KAction *m_undo, *m_redo, *m_copy, *m_cut, *m_paste;

    // dialog actions
    KToggleAction *m_dialog_gradient, *m_dialog_gradienteditor;

    // sidebar
    KToggleAction *m_side_bar;

    // tool actions
    KToggleAction *m_tool_select_rect, *m_tool_select_polygon, *m_tool_move, 
    *m_tool_zoom, *m_tool_brush, *m_tool_draw, *m_tool_pen, *m_tool_gradient, 
    *m_tool_colorpicker, *m_tool_fill, *m_tool_airbrush, *m_tool_eraser;

    KisDoc                *m_pDoc;
    KisTool               *m_pTool; // currently active tool
    SelectTool            *m_pSelectTool;
    MoveTool              *m_pMoveTool;
    BrushTool             *m_pBrushTool;
    EraserTool            *m_pEraserTool;
    AirBrushTool          *m_pAirBrushTool;
    PenTool               *m_pPenTool;
    ZoomTool              *m_pZoomTool;
    KisGradient           *m_pGradient;
    GradientTool          *m_pGradientTool;
    ColorPicker           *m_pColorPicker;
    const KisBrush        *m_pBrush; // current brush

    // sidebar dock widgets
    KisBrushChooser      *m_pBrushChooser;
    KisLayerView         *m_pLayerView;
    KisChannelView       *m_pChannelView;

    GradientDialog       *m_pGradientDialog;
    GradientEditorDialog *m_pGradientEditorDialog;

    KisCanvas            *m_pCanvas;
    //KisCanvas            *pDrawCanvas;
    KisSideBar           *m_pSideBar;
    QScrollBar           *m_pHorz, *m_pVert;
    KRuler               *m_pHRuler, *m_pVRuler;
    KisColor             m_fg, m_bg;

    KisTabBar            *m_pTabBar;
    QButton              *m_pTabFirst, *m_pTabLeft, *m_pTabRight, *m_pTabLast;

    KHelpMenu            *m_helpMenu;

    float		 m_zoomFactor;
};

#endif
