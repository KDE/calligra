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

#include <qcursor.h>
#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
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

#include <kdebug.h>
#include <klocale.h>
#include <kcolormimedata.h>

int
VCanvas::pageOffsetX() const
{
	double zoomedWidth = m_part->document().width() * m_view->zoom();
	if( contentsWidth() < visibleWidth() )
		return int( 0.5 * ( visibleWidth() - zoomedWidth ) );
	else
		return int( 0.5 * ( contentsWidth() - zoomedWidth ) );
}

int
VCanvas::pageOffsetY() const
{
	double zoomedHeight = m_part->document().height() * m_view->zoom();
	if( contentsHeight() < visibleHeight() )
		return int( 0.5 * ( visibleHeight() - zoomedHeight ) );
	else
		return int( 0.5 * ( contentsHeight() - zoomedHeight ) );
}

QPointF VCanvas::snapToGrid( const QPointF &point )
{
	if( !m_part->document().grid().isSnap )
		return point;

	QPointF p = point;

	QSizeF dist = m_part->document().grid().snap;
	QSizeF dxy = m_part->document().grid().freq;

	int dx = qRound( p.x() / dxy.width() );
	int dy = qRound( p.y() / dxy.height() );

	float distx = qMin( QABS( p.x() - dxy.width() * dx ), QABS( p.x() - dxy.width() * ( dx + 1 ) ) );
	float disty = qMin( QABS( p.y() - dxy.height() * dy ), QABS( p.y() - dxy.height() * ( dy + 1 ) ) );

	if( distx < dist.width() )
	{
		if( QABS(p.x() - dxy.width() * dx ) < QABS( p.x() - dxy.width() * ( dx + 1 ) ) )
			p.rx() = dxy.width() * dx;
		else
			p.rx() = dxy.width() * ( dx + 1 );
	}

	if( disty < dist.height() )
	{
		if( QABS( p.y() - dxy.height() * dy ) < QABS( p.y() - dxy.height() * ( dy + 1 ) ) )
			p.ry() = dxy.height() * dy;
		else
			p.ry() = dxy.height() * ( dy + 1 );
	}

	return p;
}


VCanvas::VCanvas( QWidget *parent, KarbonView* view, KarbonPart* part )
    : Q3ScrollView( parent, "canvas", Qt::WStaticContents/*WNorthWestGravity*/ | Qt::WResizeNoErase  |
	  Qt::WNoAutoErase ), m_part( part ), m_view( view )
{
	connect(this, SIGNAL( contentsMoving( int, int ) ), this, SLOT( slotContentsMoving( int, int ) ) );
	viewport()->setFocusPolicy( Qt::StrongFocus );

	viewport()->setMouseTracking( true );
	setMouseTracking( true );

	QPalette p = viewport()->palette();
	QBrush b(QColor("white"), Qt::NoBrush);
	p.setBrush(QPalette::Window, b);
	// TODO Check if the code above is ok when karbon actually runs.
	//viewport()->setBackgroundColor( Qt::white );
	//viewport()->setBackgroundMode( Qt::NoBackground );
	viewport()->installEventFilter( this );

	resizeContents( 800, 600 );
	m_pixmap = new QPixmap( 800, 600 );

	setFocus();

	setAcceptDrops( true );
}

VCanvas::~VCanvas()
{
	delete m_pixmap;
	m_view = 0L;
	m_part = 0L;
}

void
VCanvas::setPos( const QPointF& p )
{
	QPointF p2 = toViewport( p );
	QCursor::setPos( mapToGlobal( p2.toPoint() ) );
}

bool
VCanvas::eventFilter( QObject* object, QEvent* event )
{
	Q3ScrollView::eventFilter( object, event );

	if( event->type() == QEvent::ShortcutOverride || event->type() == QEvent::Shortcut )
		return Q3ScrollView::eventFilter( object, event );

	if( event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease )
		return m_view->keyEvent( event );

	QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>( event );

	if( mouseEvent && m_view )
	{
		QPointF canvasCoordinate = toContents( QPointF( mouseEvent->pos() ) );
		return m_view->mouseEvent( mouseEvent, canvasCoordinate );
	}

	return false;
}


// This causes a repaint normally, so just overwriting it omits the repainting
void
VCanvas::focusInEvent( QFocusEvent * )
{
}

QPointF
VCanvas::toViewport( const QPointF &p ) const
{
	QPointF p2 = p;
	p2.setX( ( p.x() * m_view->zoom() ) - contentsX() + pageOffsetX() );
	if( contentsHeight() > height() )
		p2.setY( ( contentsHeight() - ( p.y() * m_view->zoom() + contentsY() + pageOffsetY() ) ) );
	else
		p2.setY( ( height() - p.y() * m_view->zoom() + pageOffsetY() ) );
	return p2;
}

QPointF
VCanvas::toContents( const QPointF &p ) const
{
	QPointF p2 = p;
	p2.setX( ( p.x() + contentsX() - pageOffsetX() ) / m_view->zoom() );
	if( contentsHeight() > height() )
		p2.setY( ( contentsHeight() - ( p.y() + contentsY() + pageOffsetY()) ) / m_view->zoom() );
	else
		p2.setY( ( height() - p.y() - pageOffsetY() ) / m_view->zoom() );
	return p2;
}

QRectF
VCanvas::boundingBox() const
{
	QPointF p1( 0, 0 );
	QPointF p2( width(), height() );
	if( !m_view->documentDeleted() )
	{
		p1 = toContents( p1 );
		p2 = toContents( p2 );
	}
	return QRectF( p1, QSizeF(p2.x(), p2.y()) ).normalized();
}

void
VCanvas::setYMirroring( VPainter *p )
{
	QMatrix mat;

	mat.scale( 1, -1 );
	mat.translate( pageOffsetX(), pageOffsetY() );

	if( contentsHeight() > visibleHeight() )
		mat.translate( -contentsX(), contentsY() - contentsHeight() );
	else
		mat.translate( 0, -visibleHeight() );

	p->setMatrix( mat );
}

void
VCanvas::viewportPaintEvent( QPaintEvent *e )
{
	QRectF rect = e->rect();
	
	setYMirroring( m_view->painterFactory()->editpainter() );
	viewport()->setUpdatesEnabled( false );
	VPainter *p = m_view->painterFactory()->painter();

	// TODO : only update ROIs
	p->begin();
	p->clear( rect, QColor( 195, 194, 193 ) );
	p->setZoomFactor( m_view->zoom() );
	setYMirroring( p );
	
	// TRICK : slightly adjust the matrix so libart AA looks better
	QMatrix mat = p->worldMatrix();
	p->setMatrix( mat.translate( -.5, -.5 ) );

	// set up clippath
	p->newPath();
	p->moveTo( rect.topLeft() );
	p->lineTo( rect.topRight() );
	p->lineTo( rect.bottomRight() );
	p->lineTo( rect.bottomLeft() );
	p->lineTo( rect.topLeft() );
	p->setClipPath();

	m_part->document().drawPage( p, m_part->pageLayout(), m_view->showPageMargins() );
	QRectF bbox = boundingBox();
	m_part->document().draw( p, &bbox );

	p->resetClipPath();
	p->end();

	// draw handle:
	VQPainter qpainter( p->device() );
	setYMirroring( &qpainter );
	qpainter.setZoomFactor( m_view->zoom() );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	if( m_view->toolController()->currentTool() )
		m_view->toolController()->currentTool()->draw( &qpainter );

	/* TODO: Replace bitBlt with these two lines when ported to qpainter
	QPainter p2(viewport());
	p2.drawPixmap(rect.topLeft().toPoint(), p, rect.toRect());
	*/
	bitBlt( viewport(), rect.topLeft().toPoint(), p->device(), rect.toRect() );

	viewport()->setUpdatesEnabled( true );
}

void
VCanvas::setViewport( double centerX, double centerY )
{
	setContentsPos( int( centerX * contentsWidth() - 0.5 * visibleWidth() ),
					int( centerY * contentsHeight() - 0.5 * visibleHeight() ) );
}

void
VCanvas::setViewportRect( const QRectF &r )
{
	viewport()->setUpdatesEnabled( false );
	double zoomX = m_view->zoom() * ( ( visibleWidth() / m_view->zoom() ) / r.width() );
	double zoomY = m_view->zoom() * ( ( visibleHeight() / m_view->zoom() ) / r.height() );
	double pageOffX = ( contentsWidth() - ( m_part->document().width() * m_view->zoom() ) ) / 2.0;
	double centerX = double( ( r.center().x() ) * m_view->zoom() + pageOffX ) / double( contentsWidth() );
	double pageOffY = ( contentsHeight() - ( m_part->document().height() * m_view->zoom() ) ) / 2.0;
	double centerY = double( ( r.center().y() ) * m_view->zoom() + pageOffY ) / double( contentsHeight() );
	double zoom = zoomX < zoomY ? zoomX : zoomY;
	resizeContents( int( ( zoom / m_view->zoom() ) * contentsWidth() ),
					int( ( zoom / m_view->zoom() ) * contentsHeight() ) );
	setViewport( centerX, 1.0 - centerY );
	m_view->setZoomAt( zoom );
	viewport()->setUpdatesEnabled( true );
}

void
VCanvas::drawContents( QPainter* painter, int clipx, int clipy,
	int clipw, int cliph  )
{
	drawDocument( painter, QRectF( clipx, clipy, clipw, cliph ) );
}

void
VCanvas::drawDocument( QPainter* /*painter*/, const QRectF&, bool drawVObjects )
{
	setYMirroring( m_view->painterFactory()->editpainter() );

	VPainter* p = m_view->painterFactory()->painter();
	if( drawVObjects )
	{
		p->begin();
		p->clear( QColor( 195, 194, 193 ) );
		p->setZoomFactor( m_view->zoom() );
		setYMirroring( p );
		// TRICK : slightly adjust the matrix so libart AA looks better
		QMatrix mat = p->worldMatrix();
		p->setMatrix( mat.translate( -.5, -.5 ) );

		m_part->document().drawPage( p, m_part->pageLayout(), m_view->showPageMargins() );
		QRectF r2 = boundingBox();
		m_part->document().draw( p, &r2 );

		p->end();
	}

	// draw handle:
	VQPainter qpainter( p->device() );
	setYMirroring( &qpainter );
	qpainter.setZoomFactor( m_view->zoom() );
	m_part->document().selection()->draw( &qpainter, m_view->zoom() );

	if( m_view->toolController()->currentTool() )
		m_view->toolController()->currentTool()->draw( &qpainter );

	/* TODO: Replace bitBlt with these two lines when ported to qpainter
	QPainter p2(viewport());
	p2.drawPixmap(QPoint(0, 0), p, QRect(0, 0, width(), height()));
	*/
	bitBlt( viewport(), 0, 0, p->device(), 0, 0, width(), height() );
}

void
VCanvas::repaintAll( bool drawVObjects )
{
	drawDocument( 0, QRectF( 0, 0, width(), height() ), drawVObjects );
}

/// repaints just a rect area (no scrolling)
void
VCanvas::repaintAll( const QRectF &r )
{
	drawDocument( 0, r );
}

void
VCanvas::resizeEvent( QResizeEvent* event )
{
	double centerX = double( contentsX() + 0.5 * visibleWidth() ) / double( contentsWidth() );
	double centerY = double( contentsY() + 0.5 * visibleHeight() ) / double( contentsHeight() );

	Q3ScrollView::resizeEvent( event );
	if( !m_pixmap )
		m_pixmap = new QPixmap( width(), height() );
	else
		m_pixmap->scaled( width(), height() );

	/* TODO: this gives problems while starting Karbon:

Program received signal SIGSEGV, Segmentation fault.
[Switching to Thread 16384 (LWP 15982)]
VPainterFactory::painter (this=0x6100680073002f) at /home/kde4dev/kde/src/koffice/karbon/render/vpainterfactory.cc:42
42              return m_painter;
(gdb) bt
#0  VPainterFactory::painter (this=0x6100680073002f) at /home/kde4dev/kde/src/koffice/karbon/render/vpainterfactory.cc:42
#1  0x00002aaaaad4472d in VCanvas::resizeEvent (this=0x8caa50, event=0x168) at /home/kde4dev/kde/src/koffice/karbon/widgets/vcanvas.cc:376



	if(m_view->painterFactory()->painter())
	{
		VPainter *p = m_view->painterFactory()->painter();
		p->resize( width(), height() );
		p->clear( QColor( 195, 194, 193 ) );
	}*/

	setViewport( centerX, centerY );
}

void
VCanvas::slotContentsMoving( int /*x*/, int /*y*/ )
{
	emit viewportChanged();
}

void
VCanvas::dragEnterEvent( QDragEnterEvent *e )
{
	e->setAccepted( KarbonDrag::canDecode( e ) || KColorMimeData::canDecode( e->mimeData() ) );
}

void
VCanvas::dropEvent( QDropEvent *e )
{
	m_view->dropEvent( e );
}

#include "vcanvas.moc"

