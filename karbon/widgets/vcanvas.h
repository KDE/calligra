/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VCANVAS_H__
#define __VCANVAS_H__


#include <QResizeEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPaintEvent>
#include <QFocusEvent>
#include <QWidget>
#include <QList>

#include <koffice_export.h>
#include <KoCanvasBase.h>
#include <KoZoomHandler.h>
#include <KoViewConverter.h>
#include <KoShape.h>

#include <QBrush>
#include <QPainter>

class QPointF;
class QRectF;
class KoViewConverter;
class KoShapeManager;
class KoTool;
class KCommand;
class KCommandHistory;
class VDocument;

class KarbonCanvas: public QWidget, public KoCanvasBase
{
    Q_OBJECT

public:
    KarbonCanvas( VDocument &document );
    virtual ~KarbonCanvas();

    void gridSize(double *horizontal, double *vertical) const;
    bool snapToGrid() const { return m_snapToGrid; }

    void addCommand(KCommand *command, bool execute = true);

    KoShapeManager *shapeManager() const { return m_shapeManager; }

    /**
     * Tell the canvas repaint the specified rectangle. The coordinates
     * are document coordinates, not view coordinates.
     */
    void updateCanvas(const QRectF& rc);

    KoTool *tool() { return m_tool; }
    void setTool(KoTool *tool) { m_tool = tool; }

    KoViewConverter *viewConverter() { return &m_zoomHandler; }

    QWidget *canvasWidget() { return this; }

    virtual KoUnit::Unit unit() { return m_unit; };

    void setCommandHistory( KCommandHistory* history ) { m_commandHistory = history; }

    /**
     * Tell the canvas that it has to adjust its size.
     * The new size depends on the current document size and the actual zoom factor. 
     * If the new calculated size is smaller than the visible size set
     * by setVisibleSize, the visible size is used as the new size.
     */
    void adjustSize();

    /** Sets the available visible size. */
    void setVisibleSize( int visibleWidth, int visibleHeight );

protected:
    void paintEvent(QPaintEvent * ev);
    void mouseEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent (QKeyEvent *e);
    void keyPressEvent (QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    KoShapeManager* m_shapeManager;
    KoZoomHandler m_zoomHandler;

    KCommandHistory *m_commandHistory;

    KoTool *m_tool;
    KoUnit::Unit m_unit;

    bool m_snapToGrid;

    VDocument *m_doc;
    QRectF m_contentRect;
    QRectF m_documentRect;
    int m_marginX;
    int m_marginY;
    int m_visibleWidth;
    int m_visibleHeight;
};

#endif
