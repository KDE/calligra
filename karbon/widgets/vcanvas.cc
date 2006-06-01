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

// Uncomment the #define below to print lots of debug information about the view.
// Or use the -DKARBON_DEBUG_CANVAS flag when using cmake, so the code stays the same.
#define KARBON_DEBUG_CANVAS

#ifdef KARBON_DEBUG_CANVAS
#define debugCanvas(text) kDebug() << "KARBON_DEBUG_CANVAS: " << text << endl
#else
#define debugCanvas(text)
#endif

int
VCanvas::pageOffsetX() const
{
	debugCanvas("VCanvas::pageOffsetX()");

	double zoomedWidth = m_part->document().width() * m_view->zoom();
	double contentsWidth = contentsRect().width();
	double visibleWidth = visibleRegion().boundingRect().width();

	if( contentsWidth < visibleWidth )
		return int( 0.5 * ( visibleWidth - zoomedWidth ) );
	else
		return int( 0.5 * ( contentsWidth - zoomedWidth ) );
}

int
VCanvas::pageOffsetY() const
{
	debugCanvas("VCanvas::pageOffsetY()");

	double zoomedHeight = m_part->document().height() * m_view->zoom();
	double contentsHeight = contentsRect().height();
	double visibleHeight = visibleRegion().boundingRect().height();

	if( contentsHeight < visibleHeight )
		return int( 0.5 * ( visibleHeight - zoomedHeight ) );
	else
		return int( 0.5 * ( contentsHeight - zoomedHeight ) );
}

QPointF VCanvas::snapToGrid( const QPointF &point )
{
	debugCanvas(QString("VCanvas::snapToGrid(QPointF(%1, %2))").arg(point.x()).arg(point.y()));

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
    : QScrollArea( parent /*, Qt::WStaticContents / * WNorthWestGravity * / | Qt::WResizeNoErase  |
	  Qt::WNoAutoErase*/ ), m_part( part ), m_view( view )
{
	debugCanvas("VCanvas::VCanvas(...)");

	setObjectName("canvas");

	setWidget(new QWidget);

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

	widget()->resize( 800, 600 );
	m_pixmap = new QPixmap( 800, 600 );

	setFocus();

	setAcceptDrops( true );
}

VCanvas::~VCanvas()
{
	debugCanvas("VCanvas::~VCanvas()");

	delete m_pixmap;
	m_view = 0L;
	m_part = 0L;
}

void
VCanvas::setPos( const QPointF& p )
{
	debugCanvas(QString("VCanvas::setPos(QPointF(%1, %2))").arg(p.x()).arg(p.y()));

	QPointF p2 = toViewport( p );
	QCursor::setPos( mapToGlobal( p2.toPoint() ) );
}

bool
VCanvas::eventFilter( QObject* object, QEvent* event )
{
	debugCanvas("VCanvas::eventFilter(...)");

	// TODO: the line below causes massive redraws.
	// Q3ScrollView::eventFilter( object, event );

	if( event->type() == QEvent::ShortcutOverride || event->type() == QEvent::Shortcut )
		return QScrollArea::eventFilter( object, event );

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
	debugCanvas("VCanvas::focusInEvent()");
}

QPointF
VCanvas::toViewport( const QPointF &p ) const
{
	debugCanvas(QString("VCanvas::toViewport(QPointF(%1, %2))").arg(p.x()).arg(p.y()));

	QPointF p2 = p;
	p2.setX( ( p.x() * m_view->zoom() ) - contentsRect().x() + pageOffsetX() );
	if( contentsRect().height() > height() )
		p2.setY( ( contentsRect().height() - ( p.y() * m_view->zoom() + contentsRect().y() + pageOffsetY() ) ) );
	else
		p2.setY( ( height() - p.y() * m_view->zoom() + pageOffsetY() ) );
	return p2;
}

QPointF
VCanvas::toContents( const QPointF &p ) const
{
	debugCanvas(QString("VCanvas::toContents(QPointF(%1, %2))").arg(p.x()).arg(p.y()));

	QPointF p2 = p;
	p2.setX( ( p.x() + contentsRect().x() - pageOffsetX() ) / m_view->zoom() );
	if( contentsRect().height() > height() )
		p2.setY( ( contentsRect().height() - ( p.y() + contentsRect().y() + pageOffsetY()) ) / m_view->zoom() );
	else
		p2.setY( ( height() - p.y() - pageOffsetY() ) / m_view->zoom() );
	return p2;
}

QRectF
VCanvas::boundingBox() const
{
	debugCanvas("VCanvas::boundingBox()");

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
	debugCanvas("VCanvas::setYMirroring()");

	QMatrix mat;

	mat.scale( 1, -1 );
	mat.translate( pageOffsetX(), pageOffsetY() );

	if( contentsRect().height() > visibleRegion().boundingRect().height() )
		mat.translate( -contentsRect().x(), contentsRect().y() - contentsRect().height() );
	else
		mat.translate( 0, -visibleRegion().boundingRect().height() );

	p->setMatrix( mat );
}

void
VCanvas::viewportPaintEvent( QPaintEvent *e )
{
	debugCanvas("VCanvas::viewportPaintEvent()");

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
	debugCanvas(QString("VCanvas::setViewport(%1, %2)").arg(centerX).arg(centerY));

	widget()->move( int( centerX * contentsRect().width() - 0.5 * visibleRegion().boundingRect().width() ),
					int( centerY * contentsRect().height() - 0.5 * visibleRegion().boundingRect().height() ) );
}

void
VCanvas::setViewportRect( const QRectF &r )
{
	debugCanvas(QString("VCanvas::setViewportRect(QRectF(%1, %2, %3, %4))").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));

	viewport()->setUpdatesEnabled( false );
	double zoomX = m_view->zoom() * ( ( visibleRegion().boundingRect().width() / m_view->zoom() ) / r.width() );
	double zoomY = m_view->zoom() * ( ( visibleRegion().boundingRect().height() / m_view->zoom() ) / r.height() );
	double pageOffX = ( contentsRect().width() - ( m_part->document().width() * m_view->zoom() ) ) / 2.0;
	double centerX = double( ( r.center().x() ) * m_view->zoom() + pageOffX ) / double( contentsRect().width() );
	double pageOffY = ( contentsRect().height() - ( m_part->document().height() * m_view->zoom() ) ) / 2.0;
	double centerY = double( ( r.center().y() ) * m_view->zoom() + pageOffY ) / double( contentsRect().height() );
	double zoom = zoomX < zoomY ? zoomX : zoomY;
	widget()->resize( int( ( zoom / m_view->zoom() ) * contentsRect().width() ),
					int( ( zoom / m_view->zoom() ) * contentsRect().height() ) );
	setViewport( centerX, 1.0 - centerY );
	m_view->setZoomAt( zoom );
	viewport()->setUpdatesEnabled( true );
}

void
VCanvas::drawContents( QPainter* painter, int clipx, int clipy,
	int clipw, int cliph  )
{
	debugCanvas("VCanvas::drawContents(...)");

	drawDocument( painter, QRectF( clipx, clipy, clipw, cliph ) );
}

void
VCanvas::drawDocument( QPainter* /*painter*/, const QRectF&, bool drawVObjects )
{
	debugCanvas(QString("VCanvas::drawDocument(painter, QRectF, drawVObjects = %1").arg(drawVObjects));

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
	debugCanvas("VCanvas::repaintAll(...)");

	drawDocument( 0, QRectF( 0, 0, width(), height() ), drawVObjects );
}

/// repaints just a rect area (no scrolling)
void
VCanvas::repaintAll( const QRectF &r )
{
	debugCanvas("VCanvas::repaintAll(QRectF)");

	drawDocument( 0, r );
}

void
VCanvas::resizeEvent( QResizeEvent* event )
{
	debugCanvas("VCanvas::resizeEvent()");

	double centerX = double( contentsRect().x() + 0.5 * visibleRegion().boundingRect().width() ) / double( contentsRect().width() );
	double centerY = double( contentsRect().y() + 0.5 * visibleRegion().boundingRect().height() ) / double( contentsRect().height() );

	QScrollArea::resizeEvent( event );
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
	debugCanvas("VCanvas::slotContentsMoving()");

	emit viewportChanged();
}

void
VCanvas::dragEnterEvent( QDragEnterEvent *e )
{
	debugCanvas("VCanvas::dragEnterEvent()");

	e->setAccepted( KarbonDrag::canDecode( e->mimeData() ) || KColorMimeData::canDecode( e->mimeData() ) );
}

void
VCanvas::dropEvent( QDropEvent *e )
{
	debugCanvas("VCanvas::dropEvent()");

	m_view->dropEvent( e );
}

double VCanvas::contentsX()
{
	return contentsRect().x();
}

double VCanvas::contentsY()
{
	return contentsRect().y();
}

double VCanvas::contentsWidth()
{
	return contentsRect().width();
}

double VCanvas::contentsHeight()
{
	return contentsRect().height();
}

double VCanvas::visibleWidth()
{
	return visibleRegion().boundingRect().width();
}

double VCanvas::visibleHeight()
{
	return visibleRegion().boundingRect().height();
}

void VCanvas::resizeContents (int width, int height)
{
	widget()->resize(width, height);
}

void VCanvas::scrollContentsBy(int dx, int dy)
{
	widget()->scroll(dx, dy);
}

#include "vcanvas.moc"

