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

#include <qpainter.h>
#include <qpixmap.h>
#include <QDropEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <QSizeF>

#include "karbon_view.h"
#include "karbon_part.h"
#include "karbon_drag.h"
#include "vcanvas.h"
#include "vdocument.h"
#include "vpainter.h"
#include "vqpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vtoolcontroller.h"
#include "vtool.h"

#include "KoZoomHandler.h"
#include "KoShapeManager.h"
#include "KoTool.h"
#include "KoPathShape.h"
#include "KoRectangleShape.h"
#include <QLinearGradient>
#include "KoLineBorder.h"
#include "KoShapeGroup.h"
#include "KoPointerEvent.h"

#include <kdebug.h>
#include <klocale.h>
#include <kcolormimedata.h>
#include "vlayer.h"

KarbonCanvas::KarbonCanvas(const QList<KoShape*> &objects)
    : QWidget()
    , m_tool(0)
    , m_zoomHandler()
    , m_snapToGrid(false)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_shapeManager = new KoShapeManager(this, objects);
    setMouseTracking(true);

    //connect( m_shapeManager, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()) );
    setFocusPolicy(Qt::ClickFocus); // allow to receive keyboard input
    setMinimumSize(800, 600);
}

KarbonCanvas::~KarbonCanvas()
{
}

void KarbonCanvas::paintEvent(QPaintEvent * ev)
{
    QPainter gc( this );
    gc.setRenderHint(QPainter::Antialiasing);
    gc.setClipRect(ev->rect());

    m_shapeManager->paint( gc, m_zoomHandler, false );
    if( m_tool )
        m_tool->paint( gc, m_zoomHandler );

    gc.end();
}

void KarbonCanvas::wheelEvent(QWheelEvent *e)
{
    double steps = e->delta() / 240.0;
    // TODO: zoomhandler doesn't have a zoom() function, not sure how to replace it at the moment, so disable it till it's fixed (tbscope)
    // int zoom = qRound(m_zoomHandler.zoom() / (float) pow(2.0, steps));
    //zoomChanged( qMax(10, qMin(1000, zoom)) );
}

void KarbonCanvas::mouseMoveEvent(QMouseEvent *e)
{
    KoPointerEvent ev(e, QPointF( m_zoomHandler.viewToDocument(e->pos()) ));
    m_tool->mouseMoveEvent( &ev );
}

void KarbonCanvas::mousePressEvent(QMouseEvent *e)
{
    KoPointerEvent ev(e, QPointF( m_zoomHandler.viewToDocument(e->pos()) ));
    m_tool->mousePressEvent( &ev );
}

void KarbonCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    KoPointerEvent ev(e, QPointF( m_zoomHandler.viewToDocument(e->pos()) ));
    m_tool->mouseReleaseEvent( &ev );
}

void KarbonCanvas::keyReleaseEvent (QKeyEvent *e) {
    m_tool->keyReleaseEvent(e);
}

void KarbonCanvas::keyPressEvent (QKeyEvent *e) {
    m_tool->keyPressEvent(e);
}

void KarbonCanvas::gridSize(double *horizontal, double *vertical) const {
    *horizontal = 14.173; // approx 0.5 cm grid size.
    *vertical = 14.173;
}

void KarbonCanvas::addCommand(KCommand *command, bool execute) {
    //m_commandHistory->addCommand(command, execute);
}

void KarbonCanvas::updateCanvas(const QRectF& rc) {
    QRect clipRect(m_zoomHandler.documentToView(rc).toRect());
    clipRect.adjust(-2, -2, 2, 2); // grow for to anti-aliasing
    update(clipRect);
}

#include "vcanvas.moc"

