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
#include <qpainter.h>
#include <qpixmap.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vcanvas.h"
#include "vpainter.h"
#include "vqpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vlayersdocker.h"

#include <kdebug.h>


VCanvas::VCanvas( KarbonView* view, KarbonPart* part )
    : QScrollView( view, "canvas", WStaticContents/*WNorthWestGravity*/ | WResizeNoErase  |
	  WRepaintNoErase ), m_part( part ), m_view( view )
{
	connect(this, SIGNAL( contentsMoving( int, int ) ), this, SLOT( slotContentsMoving( int, int ) ) );
	viewport()->setFocusPolicy( QWidget::StrongFocus );

	viewport()->setMouseTracking( true );
	setMouseTracking( true );

	viewport()->setBackgroundColor( Qt::white );
	viewport()->setBackgroundMode( QWidget::NoBackground );

	resizeContents( 800, 600 );
	m_pixmap = new QPixmap( 800, 600 );

	setFocus();

	m_bScrolling = false;

}

// This causes a repaint normally, so just overwriting it omits the repainting
void
VCanvas::focusInEvent( QFocusEvent * )
{
}

void
VCanvas::toContents( int vx, int vy, int x, int y ) const
{
	x = vx + int( contentsX() * m_view->zoom() );
	y = vy + contentsY();// - ( contentsHeight() / m_view->zoom() ) + y;
	//QScrollView::viewportToContents( vx, vy, x, y );
}

KoPoint
VCanvas::toContents( const KoPoint &p ) const
{
	KoPoint p2 = p;
	//kdDebug() << p.y() << endl;
	p2.setX( ( p.x() + contentsX() ) / m_view->zoom() );
	//p2.setY( ( contentsHeight() / m_view->zoom() - ( p.y() + contentsY() / m_view->zoom() ) ) );
	if( contentsHeight() > height() )
		p2.setY( ( contentsHeight() - ( p.y() + contentsY() ) ) / m_view->zoom() );
	else
		p2.setY( ( height() - p.y() ) / m_view->zoom() );
	//kdDebug() << "contentsHeight() : " << contentsHeight() << endl;
	//kdDebug() << "p.y() : " << p.y() << endl;
	//kdDebug() << "contentsY() : " << contentsY() << endl;
	//p2.setY( ( contentsHeight() - ( p.y() + contentsY() ) * m_view->zoom() ) ) / m_view->zoom() );
	//kdDebug() << p2.y() << endl;
	return p2;
}

void
VCanvas::setYMirroring( bool edit )
{
	VPainter *p;
	QWMatrix mat;
	//kdDebug() << "viewport()->height() : " << contentsHeight() << endl;
	if( edit )
	{
		p = m_view->painterFactory()->editpainter();
		mat.scale( 1, -1 );
	}
	else
	{
		p = m_view->painterFactory()->painter();
		mat.scale( 1, -1 );
	}
	if( contentsHeight() > height() )
		mat.translate( -contentsX(), contentsY() - contentsHeight() );
	else
		mat.translate( 0, -height() );
	p->setWorldMatrix( mat );
}

void
VCanvas::viewportPaintEvent( QPaintEvent *e )
{
	//kdDebug() << " e->rect() : " << e->rect().x() << ", " << e->rect().y() << ", " << e->rect().width() << ", " << e->rect().height() << endl;
	KoRect rect( e->rect().x() - 1, e->rect().y() - 2, e->rect().width() + 2, e->rect().height() + 4 );
	VPainter *p = m_view->painterFactory()->painter();
	if( m_bScrolling )
	{
		// TODO : only update ROIs
		KoRect r( 0, 0, viewport()->width(), viewport()->height() );
		p->begin();
		p->clear( QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( false );

		m_part->document().drawPage( p );
		m_part->document().draw( p, &r );

		m_bScrolling = false;

	}
	p->blit( rect );

	// draw handle:
	VQPainter qpainter( p->device() );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	if( contentsHeight() > height() )
		mat.translate( -contentsX(), contentsY() - contentsHeight() );
	else
		mat.translate( 0, -height() );
	qpainter.setWorldMatrix( mat );
	qpainter.setZoomFactor( m_view->zoom() );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	bitBlt( viewport(), QPoint( rect.x(), rect.y() ), p->device(), rect.toQRect() );
	//bitBlt( this, QPoint( rect.x(), rect.y() - 20 ), p->device(), rect );
}

void
VCanvas::drawContents( QPainter* painter, int clipx, int clipy,
	int clipw, int cliph  )
{
	//kdDebug() << "VCanvas::drawContents" << endl;
	drawDocument( painter, KoRect( clipx, clipy, clipw, cliph ) );
}

void
VCanvas::drawDocument( QPainter* /*painter*/, const KoRect& rect, bool drawVObjects )
{
	//kdDebug() << "drawDoc rect : " << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << endl;
	VPainter* p = m_view->painterFactory()->painter();
	if( drawVObjects )
	{
		p->begin();
		p->clear( QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( false );

		m_part->document().drawPage( p );
		m_part->document().draw( p, &rect );

		p->end();
	}

	// draw handle:
	VQPainter qpainter( p->device() );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	if( contentsHeight() > height() )
		mat.translate( -contentsX(), contentsY() - contentsHeight() );
	else
		mat.translate( 0, -height() );
	qpainter.setWorldMatrix( mat );
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
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	if( contentsHeight() > height() )
		mat.translate( -contentsX(), contentsY() - contentsHeight() );
	else
		mat.translate( 0, -height() );
	qpainter.setWorldMatrix( mat );
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
}

#include <vcanvas.moc>
