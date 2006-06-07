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


#include <QAbstractScrollArea>
#include <QPixmap>
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
#include <kcommand.h>

#include <KoShapeContainer.h>

#include <QColor>
#include <QRect>
#include <QBrush>
#include <QPainter>

class QPointF;
class QRectF;
class KarbonPart;
class KarbonView;
class VPainter;
class KoViewConverter;
class KoShapeManager;
class KoTool;

class KarbonCanvas: public QWidget, public KoCanvasBase
{
    Q_OBJECT

public:
    KarbonCanvas(const QList<KoShape*> &objects);
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

    KoViewConverter *viewConverter() { return &m_viewConverter; }

    QWidget *canvasWidget() { return this; }

protected:
    void paintEvent(QPaintEvent * ev);
    void wheelEvent(QWheelEvent *e);
    void mouseEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent (QKeyEvent *e);
    void keyPressEvent (QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    QImage buffer;

    KoShapeManager* m_shapeManager;
    KoZoomHandler m_zoomHandler;

    class ViewConverter : public KoViewConverter {
        public:
            ViewConverter(KoZoomHandler *handler) { m_zoomHandler = handler; };
            QPointF normalToView( const QPointF &normalPoint );
            QPointF viewToNormal( const QPointF &viewPoint );
            QRectF normalToView( const QRectF &normalPoint );
            QRectF viewToNormal( const QRectF &viewPoint );
            void zoom(double *zoomX, double *zoomY) const;
        private:
            KoZoomHandler *m_zoomHandler;
    };

    //KSillyCommandHistory *m_commandHistory;

    ViewConverter m_viewConverter;
    KoTool *m_tool;

    bool m_snapToGrid;
};

#endif
