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
#include "vpainterfactory.h"
#include "vselection.h"

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
	x = vx + contentsX() * m_view->zoom();;
	y = vy + contentsY();// - ( contentsHeight() / m_view->zoom() ) + y;
	//QScrollView::viewportToContents( vx, vy, x, y );
}

QPoint
VCanvas::toContents( const QPoint &p ) const
{
	QPoint p2 = p;
	//kdDebug() << p.y() << endl;
	p2.setX( ( p.x() + contentsX() ) / m_view->zoom() );
	//p2.setY( ( contentsHeight() / m_view->zoom() - ( p.y() + contentsY() / m_view->zoom() ) ) );
	p2.setY( ( contentsHeight() - ( p.y() + contentsY() ) ) / m_view->zoom() );
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
	kdDebug() << "viewport()->height() : " << contentsHeight() << endl;
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
	mat.translate( -contentsX(), contentsY() - contentsHeight() );
	p->setWorldMatrix( mat );
}

// TODO maybe this belongs in VDocument
void
drawPage( VPainter *p )
{
#define LEFT   20
#define RIGHT  550
#define BOTTOM 20
#define TOP    830

	p->setPen( Qt::black );
	p->setBrush( Qt::white );
	p->newPath();
	p->moveTo( KoPoint( LEFT,  BOTTOM ) );
	p->lineTo( KoPoint( RIGHT, BOTTOM ) );
	p->lineTo( KoPoint( RIGHT, TOP ) );
	p->lineTo( KoPoint( LEFT,  TOP ) );
	p->lineTo( KoPoint( LEFT,  BOTTOM ) );
	p->fillPath();
	p->strokePath();

	p->setPen( Qt::NoPen );
	p->setBrush( Qt::black );
	p->newPath();
	p->moveTo( KoPoint( RIGHT,     BOTTOM - 2 ) );
	p->lineTo( KoPoint( RIGHT + 2, BOTTOM - 2 ) );
	p->lineTo( KoPoint( RIGHT + 2, TOP ) );
	p->lineTo( KoPoint( RIGHT,     TOP ) );
	p->fillPath();

	p->newPath();
	p->moveTo( KoPoint( LEFT,  BOTTOM ) );
	p->lineTo( KoPoint( LEFT,  BOTTOM - 2 ) );
	p->lineTo( KoPoint( RIGHT, BOTTOM - 2 ) );
	p->lineTo( KoPoint( RIGHT, BOTTOM ) );
	p->fillPath();

	p->newPath();
	p->moveTo( KoPoint( LEFT,  TOP ) );
	p->lineTo( KoPoint( LEFT,  TOP + 1 ) );
	p->lineTo( KoPoint( RIGHT, TOP + 1 ) );
	p->lineTo( KoPoint( RIGHT, TOP ) );
	p->fillPath();
}

void
VCanvas::viewportPaintEvent( QPaintEvent *e )
{
	//kdDebug() << " e->rect() : " << e->rect().x() << ", " << e->rect().y() << ", " << e->rect().width() << ", " << e->rect().height() << endl;
	QRect rect( e->rect().x(), e->rect().y(), e->rect().width(), e->rect().height() );
	VPainter *p = m_view->painterFactory()->painter();
	if( m_bScrolling )
	{
		// TODO : only update ROIs
		KoRect r( 0, 0, viewport()->width(), viewport()->height() );
		p->begin();
		p->clear( QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( false );
		drawPage( p );

		m_part->document().draw( p, &r );

		m_bScrolling = false;

		p->blit( rect );
	}

	// draw handle:
	QPainter qpainter( p->device() );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( -contentsX(), contentsY() - contentsHeight() );
	qpainter.setWorldMatrix( mat );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	bitBlt( viewport(), QPoint( rect.x(), rect.y() ), p->device(), rect );
}

void
VCanvas::drawContents( QPainter* painter, int clipx, int clipy,
	int clipw, int cliph  )
{
	//kdDebug() << "VCanvas::drawContents" << endl;
	drawDocument( painter, QRect( clipx, clipy, clipw, cliph ) );
}

void
VCanvas::drawDocument( QPainter* /*painter*/, const QRect& rect, bool drawVObjects )
{
	//kdDebug() << "drawDoc rect : " << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << endl;
	VPainter* p = m_view->painterFactory()->painter();
	if( drawVObjects )
	{
		p->begin();
		p->clear( QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( false );
		drawPage( p );

		m_part->document().draw( p, &KoRect::fromQRect( rect ) );

		p->end();
	}

	// draw handle:
	QPainter qpainter( p->device() );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( -contentsX(), contentsY() - contentsHeight() );
	qpainter.setWorldMatrix( mat );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	bitBlt( viewport(), 0, 0, p->device(), 0, 0, width(), height() );
}

void
VCanvas::repaintAll( bool drawVObjects )
{
	//drawContents( 0, 0, 0, width(), height() );
	drawDocument( 0, QRect( 0, 0, width(), height() ), drawVObjects );
	//viewport()->repaint( erase );
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
