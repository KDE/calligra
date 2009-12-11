/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "KChartCanvas.h"

// Qt
#include <QPaintEvent>

// KDE
#include <KDebug>

// KOffice
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeCreateCommand.h>
#include <KoToolProxy.h>
#include <KoUnit.h>
#include <KoViewConverter.h>
#include <KoToolManager.h>

// KChart
#include "ChartShape.h"
#include "KChartPart.h"
#include "KChartView.h"

namespace KChart
{

KChartCanvas::KChartCanvas( KChartView *view, KChartPart *parent )
    : QWidget(),
      KoCanvasBase( parent ),
      m_origin( 0, 0 ),
      m_documentOffset( 0, 0 )
{
    m_part = parent;
    m_view = view;
    m_shapeManager = new KoShapeManager( this );
    //m_shapeManager->add( m_part->shape() );
    m_toolProxy = new KoToolProxy( this );

    setBackgroundRole( QPalette::Base );
    setAutoFillBackground( true );
    setMouseTracking( true );
    setFocusPolicy( Qt::ClickFocus ); // allow to receive keyboard input
    adjustOrigin();
    setAttribute( Qt::WA_InputMethodEnabled, true );

    m_documentRect = m_part->chart()->boundingRect();

    connect(m_shapeManager, SIGNAL( selectionChanged() ), this, SLOT( adjustOrigin() ) );
}

KChartCanvas::~KChartCanvas()
{
}

void KChartCanvas::paintEvent( QPaintEvent *ev )
{
    QPainter painter( this );
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate( -m_documentOffset );

    QRect clipRect = ev->rect().translated( m_documentOffset );
    painter.setClipRect( clipRect );
    painter.translate( m_origin );
    // KChartView::viewConverter() returns the view's KoZoomHandler instance
    shapeManager()->paint( painter, *( m_view->viewConverter() ), false );
    m_toolProxy->paint( painter, *( m_view->viewConverter() ) );

    painter.end();
}

void KChartCanvas::gridSize( qreal*, qreal* ) const
{
}

bool KChartCanvas::snapToGrid() const
{
    return false;
}

void KChartCanvas::addCommand( QUndoCommand *command )
{
    m_part->KoDocument::addCommand( command );
    adjustOrigin();
}

KoShapeManager *KChartCanvas::shapeManager() const
{
    //kDebug() << "shapeManager";
    return m_shapeManager;
}

void KChartCanvas::updateCanvas( const QRectF& clipRect )
{
    QRect rect( viewToWidget( viewConverter()->documentToView( clipRect ).toRect() ) );
    rect.adjust( -2, -2, 2, 2 ); // grow for anti-aliasing
    rect.moveTopLeft( rect.topLeft() - m_documentOffset );
    update( rect );
}

KoToolProxy *KChartCanvas::toolProxy() const
{
    return m_toolProxy;
}

const KoViewConverter *KChartCanvas::viewConverter() const
{
    return m_view->viewConverter();
}

KoUnit KChartCanvas::unit() const
{
    return KoUnit();
}

void KChartCanvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

void KChartCanvas::adjustOrigin()
{
    QRect documentRect = m_view->viewConverter()->documentToView( documentViewRect() ).toRect();

    QPoint origin = m_origin;
    
    m_origin = -documentRect.topLeft();

    int widthDiff = size().width() - documentRect.width();
    if(widthDiff > 0)
        m_origin.rx() += widthDiff;
    int heightDiff = size().height() - documentRect.height();
    if(heightDiff > 0)
        m_origin.rx() += heightDiff;

    if( origin != m_origin )
        emit documentOriginChanged( m_origin );
    update();
}

void KChartCanvas::setDocumentOffset( const QPoint &point )
{
    m_documentOffset = point;
}

QRectF KChartCanvas::documentViewRect()
{
    // TODO for now, we only have one shape. That will probably change in the future.
    // FIXME Apply viewing margin
    QRectF documentRect = m_documentRect;
    m_documentRect = m_part->chart()->boundingRect();
    m_documentRect = m_documentRect.united( QRectF( QPointF( 0.0, 0.0 ), m_documentRect.size() ) );
    if( documentRect != m_documentRect )
        emit documentViewRectChanged( m_documentRect );

    return m_documentRect;
}

QPoint KChartCanvas::widgetToView( const QPoint &point )
{
    return point - m_origin;
}

QPoint KChartCanvas::viewToWidget( const QPoint &point )
{
    return point + m_origin;
}

QRect KChartCanvas::widgetToView( const QRect &rect )
{
    return rect.translated( -m_origin );
}

QRect KChartCanvas::viewToWidget( const QRect &rect )
{
    return rect.translated( m_origin );
}

void KChartCanvas::mouseReleaseEvent( QMouseEvent *e )
{
    m_toolProxy->mouseReleaseEvent( e, m_view->viewConverter()->viewToDocument( widgetToView( e->pos() ) ) );
}

void KChartCanvas::keyReleaseEvent ( QKeyEvent *e )
{
    m_toolProxy->keyReleaseEvent( e );
}

void KChartCanvas::keyPressEvent ( QKeyEvent *e )
{
    m_toolProxy->keyPressEvent( e );
    if (! e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab
                || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (e->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

void KChartCanvas::mouseMoveEvent( QMouseEvent *e )
{
    m_toolProxy->mouseMoveEvent( e, m_view->viewConverter()->viewToDocument( widgetToView( e->pos() + m_documentOffset ) ) );
    update();
}

void KChartCanvas::mousePressEvent( QMouseEvent *e )
{
    m_toolProxy->mousePressEvent( e, m_view->viewConverter()->viewToDocument( widgetToView( e->pos() + m_documentOffset ) ) );
    update();
}

void KChartCanvas::mouseDoubleClickEvent( QMouseEvent *e )
{
    m_toolProxy->mouseDoubleClickEvent( e, m_view->viewConverter()->viewToDocument( widgetToView( e->pos() + m_documentOffset ) ) );
    update();
}

void KChartCanvas::tabletEvent( QTabletEvent *e )
{
    m_toolProxy->tabletEvent( e, m_view->viewConverter()->viewToDocument( widgetToView( e->pos() + m_documentOffset ) ) );
}

void KChartCanvas::wheelEvent( QWheelEvent *e )
{
    m_toolProxy->wheelEvent( e, m_view->viewConverter()->viewToDocument( widgetToView( e->pos() + m_documentOffset ) ) );
    update();
}

void KChartCanvas::resizeEvent( QResizeEvent *e )
{
    Q_UNUSED(e);
    adjustOrigin();
}

QVariant KChartCanvas::inputMethodQuery( Qt::InputMethodQuery query ) const
{
    return m_toolProxy->inputMethodQuery( query, *( viewConverter() ) );
}

void KChartCanvas::inputMethodEvent( QInputMethodEvent *e )
{
    m_toolProxy->inputMethodEvent( e );
}

}

#include "KChartCanvas.moc"
