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
#include "vmtool_handle.h"
#include "vpainter.h"
#include "vpainterfactory.h"

#include <kdebug.h>


VCanvas::VCanvas( KarbonView* view, KarbonPart* part )
    : QScrollView( view, "canvas", WStaticContents/*WNorthWestGravity*/ | WResizeNoErase  |
	  WRepaintNoErase ), m_part( part ), m_view( view ), m_zoomFactor( 1.0 )
{
	connect(this, SIGNAL( contentsMoving( int, int ) ), this, SLOT( slotContentsMoving( int, int ) ) );
	viewport()->setFocusPolicy( QWidget::StrongFocus );

	viewport()->setMouseTracking( true );
	setMouseTracking( true );

	viewport()->setBackgroundColor( Qt::white );
	viewport()->setBackgroundMode( QWidget::NoBackground );

// TODO: remove this line
resizeContents( 800, 600 );

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
		QRect r( 0, 0, viewport()->width(), viewport()->height() );
		p->begin();
		QWMatrix mat;
		mat.translate( -contentsX(), -contentsY() );
		p->setWorldMatrix( mat );

		QPtrListIterator<VLayer> i = m_part->layers();
		for ( ; i.current(); ++i )
			if ( i.current()->visible() )
				i.current()->draw( p, r, m_zoomFactor );

		m_bScrolling = false;
	}
	p->blit( rect );
}

void
VCanvas::drawContents( QPainter* painter, int clipx, int clipy,
	int clipw, int cliph  )
{
	//kdDebug() << "VCanvas::drawContents" << endl;
	drawDocument( painter, QRect( clipx, clipy, clipw, cliph ) );
}

void
VCanvas::drawDocument( QPainter* /*painter*/, const QRect& rect )
{
	//kdDebug() << "drawDoc rect : " << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << endl;
	VPainter* p = m_view->painterFactory()->painter();
	p->begin();
	QWMatrix mat;
	mat.translate( -contentsX(), -contentsY() );
	p->setWorldMatrix( mat );
	//VPainter *p = m_view->painterFactory()->painter( this, visibleWidth(), visibleHeight() );
	//erase( rect );

	QPtrListIterator<VLayer> i = m_part->layers();
	for ( ; i.current(); ++i )
		if ( i.current()->visible() )
			i.current()->draw( p, rect, m_zoomFactor );

	p->end();

	// draw handle:
	QPainter qpainter( p->device() );
	VMToolHandle::instance( m_part )->draw( qpainter, m_zoomFactor );
}

void
VCanvas::repaintAll( bool /*erase*/ )
{
	drawContents( 0, 0, 0, width(), height() );
	//viewport()->repaint( erase );
}

void
VCanvas::resizeEvent( QResizeEvent* event )
{
	QScrollView::resizeEvent( event );
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
