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
#include <QDropEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <QSizeF>

#include "karbon_drag.h"
#include "vcanvas.h"

#include "KoZoomHandler.h"
#include "KoShapeManager.h"
#include "KoTool.h"
#include "KoPointerEvent.h"

#include <kdebug.h>
#include <klocale.h>
#include <kcolormimedata.h>
#include <kcommand.h>

#include "vdocument.h"

const int defaultMargin = 50;

KarbonCanvas::KarbonCanvas( VDocument &document )
    : QWidget()
    , m_zoomHandler()
    , m_tool(0)
    , m_doc( &document )
    , m_marginX( defaultMargin )
    , m_marginY( defaultMargin )
    , m_visibleWidth( 500 )
    , m_visibleHeight( 500 )
    , m_fitMarginX( 20 )
    , m_fitMarginY( 20 )
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_shapeManager = new KoShapeManager(this, m_doc->shapes() );
    setMouseTracking(true);

    connect( m_shapeManager, SIGNAL(selectionChanged()), this, SLOT(adjustSize()) );
    setFocusPolicy(Qt::ClickFocus); // allow to receive keyboard input
    adjustSize();
}

KarbonCanvas::~KarbonCanvas()
{
}

void KarbonCanvas::paintEvent(QPaintEvent * ev)
{
    QPainter gc( this );
    gc.setRenderHint(QPainter::Antialiasing);
    gc.setClipRect(ev->rect());

    gc.translate( m_origin.x(), m_origin.y() );
    gc.setPen( Qt::black );
    gc.drawRect( m_zoomHandler.documentToView( m_documentRect ) );

    if( m_doc->grid().visible() )
        m_doc->grid().paint( gc, m_zoomHandler, m_zoomHandler.viewToDocument( widgetToView( ev->rect() ) ) );

    m_shapeManager->paint( gc, m_zoomHandler, false );
    m_tool->paint( gc, m_zoomHandler );

    gc.end();
}

void KarbonCanvas::mouseMoveEvent(QMouseEvent *e)
{
    KoPointerEvent ev(e, m_zoomHandler.viewToDocument( widgetToView( e->pos() ) ) );
    m_tool->mouseMoveEvent( &ev );
}

void KarbonCanvas::mousePressEvent(QMouseEvent *e)
{
    KoPointerEvent ev(e, m_zoomHandler.viewToDocument( widgetToView( e->pos() ) ) );
    m_tool->mousePressEvent( &ev );
}

void KarbonCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    KoPointerEvent ev(e, m_zoomHandler.viewToDocument( widgetToView( e->pos() ) ) );
    m_tool->mouseReleaseEvent( &ev );
}

void KarbonCanvas::keyReleaseEvent (QKeyEvent *e) {
    m_tool->keyReleaseEvent(e);
}

void KarbonCanvas::keyPressEvent (QKeyEvent *e) {
    m_tool->keyPressEvent(e);
}

void KarbonCanvas::gridSize(double *horizontal, double *vertical) const {
    m_doc->grid().spacing( horizontal, vertical );
}

bool KarbonCanvas::snapToGrid() const {
    return m_doc->grid().snapping();
}

void KarbonCanvas::addCommand(KCommand *command, bool execute) {

    if( m_commandHistory )
        m_commandHistory->addCommand(command, execute);
}

void KarbonCanvas::updateCanvas(const QRectF& rc) {
    QRect clipRect( viewToWidget( m_zoomHandler.documentToView(rc).toRect() ) );
    clipRect.adjust(-2, -2, 2, 2); // grow for to anti-aliasing
    update(clipRect);
}

void KarbonCanvas::setVisibleSize( int visibleWidth, int visibleHeight ) {
    m_visibleWidth = visibleWidth;
    m_visibleHeight = visibleHeight;
    adjustSize();
}

void KarbonCanvas::setFitMargin( int fitMarginX, int fitMarginY ) {
    m_fitMarginX = fitMarginX;
    m_fitMarginY = fitMarginY;
}

void KarbonCanvas::adjustSize() {
    m_contentRect = m_doc->boundingRect();
    m_documentRect.setRect( 0.0, 0.0, m_doc->width(), m_doc->height() );

    if( m_zoomHandler.zoomMode() == KoZoomMode::ZOOM_PAGE ) 
    {
        double zoomX = double( m_visibleWidth-2*m_fitMarginX ) / double( m_zoomHandler.resolutionX() * m_documentRect.width() );
        double zoomY = double( m_visibleHeight-2*m_fitMarginY ) / double( m_zoomHandler.resolutionY() * m_documentRect.height() );

        double zoom = 1.0;
        if(zoomX < 0.0 && zoomY > 0.0 )
            zoom = zoomY;
        else if(zoomX > 0.0 && zoomY < 0.0 )
            zoom = zoomX;
        else if(zoomX < 0.0 && zoomY < 0.0)
            zoom = 0.0001;
        else
            zoom = qMin( zoomX, zoomY );
        m_zoomHandler.setZoom( zoom );
    }
    else if( m_zoomHandler.zoomMode() == KoZoomMode::ZOOM_WIDTH ) 
    {
        double zoom = double( m_visibleWidth-2*m_fitMarginX ) / double( m_zoomHandler.resolutionX() * m_documentRect.width() );
        m_zoomHandler.setZoom( zoom );
    }

    // calculate how much space we need with the current zoomed doc size and default margins
    QRect zoomedRect = m_zoomHandler.documentToView( m_contentRect ).toRect();
    int newWidth = zoomedRect.width() + 2 * defaultMargin;
    int newHeight = zoomedRect.height() + 2 * defaultMargin;

    // if the new size is smaller as the visible size, adjust the margins
    if( newWidth < m_visibleWidth )
        m_marginX = int(0.5 * float(m_visibleWidth - zoomedRect.width()));
    else
        m_marginX = defaultMargin;
    if( newHeight < m_visibleHeight )
        m_marginY = int(0.5 * float(m_visibleHeight - zoomedRect.height()));
    else
        m_marginY = defaultMargin;

    setMinimumSize( zoomedRect.width() + 2 * m_marginX, zoomedRect.height() + 2 * m_marginY );

    m_origin.setX( m_marginX - zoomedRect.left() );
    m_origin.setY( m_marginY - zoomedRect.top() );
}

QPoint KarbonCanvas::widgetToView( const QPoint& p ) const {
    return p - m_origin;
}

QRect KarbonCanvas::widgetToView( const QRect& r ) const {
    return r.translated( - m_origin );
}

QPoint KarbonCanvas::viewToWidget( const QPoint& p ) const {
    return p + m_origin;
}

QRect KarbonCanvas::viewToWidget( const QRect& r ) const {
    return r.translated( m_origin );
}

KoUnit::Unit KarbonCanvas::unit() {
    return m_doc->unit();
}

QPoint KarbonCanvas::documentOrigin()
{
    return m_origin;
}

#include "vcanvas.moc"

