/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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
		p->setZoomFactor( m_view->zoom() );
		QWMatrix mat;
		mat.scale( 1, -1 );
		kdDebug() << "viewport()->height() : " << viewport()->height() << endl;
		mat.translate( -contentsX(), contentsY() - viewport()->height() );
		p->setWorldMatrix( mat );

		m_part->document().draw( p, r );

		m_bScrolling = false;

		p->blit( rect );
	}

	// draw handle:
	QPainter qpainter( p->device() );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( -contentsX(), contentsX() - viewport()->height() );
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
		p->setZoomFactor( m_view->zoom() );
		QWMatrix mat;
		mat.scale( 1, -1 );
		kdDebug() << "contentsHeight() : " << viewport()->height() << endl;
		mat.translate( -contentsX(), contentsY() - viewport()->height() );
		p->setWorldMatrix( mat );

		m_part->document().draw( p, KoRect::fromQRect( rect ) );

		p->end();
	}

	// draw handle:
	QPainter qpainter( p->device() );
	// Y mirroring
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( -contentsX(), contentsX() - viewport()->height() );
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
