/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qapplication.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "karbon_view_base.h"
#include "karbon_part_base.h"
#include "vcanvas.h"
#include "vdocument.h"
#include "vpainter.h"
#include "vqpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"

#include <kdebug.h>

#define PAGE_OFFSETX	20
#define PAGE_OFFSETY	20

VCanvas::VCanvas( KarbonViewBase* view, KarbonPartBase* part )
    : QScrollView( view, "canvas", WStaticContents/*WNorthWestGravity*/ | WResizeNoErase  |
	  WRepaintNoErase ), m_part( part ), m_view( view )
{
	connect(this, SIGNAL( contentsMoving( int, int ) ), this, SLOT( slotContentsMoving( int, int ) ) );
	viewport()->setFocusPolicy( QWidget::StrongFocus );

	viewport()->setMouseTracking( true );
	setMouseTracking( true );

	viewport()->setBackgroundColor( Qt::white );
	viewport()->setBackgroundMode( QWidget::NoBackground );
	viewport()->installEventFilter( this );

	resizeContents( 800, 600 );
	m_pixmap = new QPixmap( 800, 600 );

	setFocus();

	m_bScrolling = false;
}

void
VCanvas::setPos( const KoPoint& p )
{
	QCursor::setPos( p.x() * m_view->zoom(), p.y() * m_view->zoom() );
}

bool
VCanvas::eventFilter( QObject* object, QEvent* event )
{
	QScrollView::eventFilter( object, event );

	if( event->type() == QEvent::AccelOverride || event->type() == QEvent::Accel )
		return QScrollView::eventFilter( object, event );

	if( event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease )
		return m_view->keyEvent( event );

	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );

	KoPoint canvasCoordinate = toContents( KoPoint( mouseEvent->pos() ) );
	canvasCoordinate.setX( canvasCoordinate.x() - PAGE_OFFSETX / m_view->zoom() );
	canvasCoordinate.setY( canvasCoordinate.y() - PAGE_OFFSETY / m_view->zoom() );
	if( mouseEvent && m_view )
		return m_view->mouseEvent( mouseEvent, canvasCoordinate );
	else
		return false;
}


// This causes a repaint normally, so just overwriting it omits the repainting
void
VCanvas::focusInEvent( QFocusEvent * )
{
}

KoPoint
VCanvas::toContents( const KoPoint &p ) const
{
	KoPoint p2 = p;
	p2.setX( ( p.x() + contentsX() ) / m_view->zoom() );
	if( contentsHeight() > height() )
		p2.setY( ( contentsHeight() - ( p.y() + contentsY() ) ) / m_view->zoom() );
	else
		p2.setY( ( height() - p.y() ) / m_view->zoom() );
	return p2;
}

KoRect
VCanvas::boundingBox() const
{
	KoPoint p1( 0, 0 );
    p1 = toContents( p1 );
    KoPoint p2( width(), height() );
    p2 = toContents( p2 );
    return KoRect( p1, p2 ).normalize();
}

void
VCanvas::setYMirroring( VPainter *p )
{
	QWMatrix mat;

	mat.scale( 1, -1 );
	mat.translate( PAGE_OFFSETX, PAGE_OFFSETY );

	if( contentsHeight() > height() )
		mat.translate( -contentsX(), contentsY() - contentsHeight() );
	else
		mat.translate( 0, -height() );

	p->setWorldMatrix( mat );
}

void
VCanvas::viewportPaintEvent( QPaintEvent *e )
{
	setYMirroring( m_view->painterFactory()->editpainter() );
	kdDebug() << "viewp e->rect() : " << e->rect().x() << ", " << e->rect().y() << ", " << e->rect().width() << ", " << e->rect().height() << endl;
	viewport()->setUpdatesEnabled( false );
	KoRect rect( e->rect().x() - 1, e->rect().y() - 2, e->rect().width() + 2, e->rect().height() + 4 );
	VPainter *p = m_view->painterFactory()->painter();
	if( m_bScrolling )
	{
		// TODO : only update ROIs
		KoRect r( 0, 0, viewport()->width(), viewport()->height() );
		p->begin();
		p->clear( rect, QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( p );

		// set up clippath
		p->newPath();
		p->moveTo( KoPoint( rect.x(), rect.y() ) );
		p->lineTo( KoPoint( rect.right(), rect.y() ) );
		p->lineTo( KoPoint( rect.right(), rect.bottom() ) );
		p->lineTo( KoPoint( rect.x(), rect.bottom() ) );
		p->lineTo( KoPoint( rect.x(), rect.y() ) );
		p->setClipPath();

		m_part->document().drawPage( p );
		m_part->document().draw( p, &r );

		p->resetClipPath();
		m_bScrolling = false;

	}
	p->blit( rect );

	// draw handle:
	VQPainter qpainter( p->device() );
	setYMirroring( &qpainter );
	qpainter.setZoomFactor( m_view->zoom() );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	bitBlt( viewport(), QPoint( rect.x(), rect.y() ), p->device(), rect.toQRect() );
	viewport()->setUpdatesEnabled( true );
	//bitBlt( this, QPoint( rect.x(), rect.y() - PAGE_OFFSETY ), p->device(), rect );
}

void
VCanvas::setViewport( double centerX, double centerY )
{
	setContentsPos( centerX * contentsWidth() - visibleWidth() / 2,
					centerY * contentsHeight() - visibleHeight() / 2 );
}

void
VCanvas::setContentsRect( const KoRect &r )
{
	viewport()->setUpdatesEnabled( false );
	kdDebug() << r.width() << endl;
	kdDebug() << r.height() << endl;
	double zoomX = m_view->zoom() * ( ( visibleWidth() / m_view->zoom() ) / r.width() );
	double zoomY = m_view->zoom() * ( ( visibleHeight() / m_view->zoom() ) / r.height() );
	kdDebug() << "Zoom : " << zoomX << ", " << zoomY << endl;
	double centerX = double( r.center().x() * m_view->zoom() ) / double( contentsWidth() );
	double centerY = double( r.center().y() * m_view->zoom() ) / double( contentsHeight() );
	double zoom = zoomX < zoomY ? zoomX : zoomY;
	kdDebug() << "cw " << contentsWidth() << endl;
	resizeContents( ( zoom / m_view->zoom() ) * contentsWidth(),
					( zoom / m_view->zoom() ) * contentsHeight() );
	kdDebug() << "cw2 : " << contentsWidth() << endl;
	kdDebug() << "cx : " << centerX << endl;
	setViewport( centerX, 1.0 - centerY );
	m_view->setZoom( zoom );
	viewport()->setUpdatesEnabled( true );
}

void
VCanvas::drawContents( QPainter* painter, int clipx, int clipy,
	int clipw, int cliph  )
{
	drawDocument( painter, KoRect( clipx, clipy, clipw, cliph ) );
}

void
VCanvas::drawDocument( QPainter* /*painter*/, const KoRect& rect, bool drawVObjects )
{
	setYMirroring( m_view->painterFactory()->editpainter() );
	//kdDebug() << "drawDoc rect : " << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << endl;
	VPainter* p = m_view->painterFactory()->painter();
	if( drawVObjects )
	{
		p->begin();
		p->clear( QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( p );

		m_part->document().drawPage( p );
		m_part->document().draw( p, &rect );

		p->end();
	}

	// draw handle:
	VQPainter qpainter( p->device() );
	setYMirroring( &qpainter );
	qpainter.setZoomFactor( m_view->zoom() );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	bitBlt( viewport(), 0, 0, p->device(), 0, 0, width(), height() );
}

void
VCanvas::repaintAll( bool drawVObjects )
{
	//if( m_view->layersDocker() )
	//	m_view->layersDocker()->updatePreviews();
	//drawContents( 0, 0, 0, width(), height() );
	drawDocument( 0, KoRect( 0, 0, width(), height() ), drawVObjects );
	//viewport()->repaint( erase );
}

/// repaints just a rect area (no scrolling)
void
VCanvas::repaintAll( const KoRect & )
{
	//if( m_view->layersDocker() )
//		m_view->layersDocker()->updatePreviews();
	VPainter *p = m_view->painterFactory()->painter();
	KoRect rect( rect().x(), rect().y(), rect().width(), rect().height() );
	p->blit( rect );

	// draw handle:
	VQPainter qpainter( p->device() );
	setYMirroring( &qpainter );
	qpainter.setZoomFactor( m_view->zoom() );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	bitBlt( viewport(), QPoint( rect.x(), rect.y() ), p->device(), rect.toQRect() );
}

void
VCanvas::resizeEvent( QResizeEvent* event )
{
	QScrollView::resizeEvent( event );
	if( !m_pixmap )
		m_pixmap = new QPixmap( width(), height() );
	else
		m_pixmap->resize( width(), height() );
	drawContents( 0, 0, 0, width(), height() );
    //VPainter *p = m_view->painterFactory()->painter();
    //p->end();
}

void
VCanvas::slotContentsMoving( int /*x*/, int /*y*/ )
{
	m_bScrolling = true;
	emit viewportChanged();
}

#include <vcanvas.moc>
