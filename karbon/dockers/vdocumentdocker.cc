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

#include <QLayout>
#include <QLabel>
#include <QCursor>
#include <QGridLayout>
#include <QPixmap>
#include <QFrame>
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QRectF>
#include <QPalette>
#include <QBrush>
#include <QPainter>

#include <klocale.h>
#include <kglobal.h>
#include <KoMainWindow.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "vdocument.h"
#include "vqpainter.h"
#include "vselection.h"
#include "vstroke.h"
#include "vcanvas.h"
#include "vdocumentdocker.h"

/*************************************************************************
 *  Document tab                                                         *
 *************************************************************************/

VDocumentPreview::VDocumentPreview( KarbonView* view, QWidget* parent )
		: QWidget( parent ), m_document( &view->part()->document() ), m_view( view )
{
	setObjectName("DocumentPreview");

	update();
	installEventFilter( this );
	QBrush b(Qt::NoBrush);
	QPalette p = palette();
	p.setBrush(QPalette::Window, b);
	setMouseTracking( true );
	m_dragging = false;
	m_docpixmap = 0L;
} // VDocumentPreview::VDocumentPreview

VDocumentPreview::~VDocumentPreview()
{
	delete m_docpixmap;
} // VDocumentPreview::~VDocumentPreview

void
VDocumentPreview::reset()
{
	delete m_docpixmap;
	m_docpixmap = 0L;
}

bool
VDocumentPreview::eventFilter( QObject* object, QEvent* event )
{
	double scaleFactor;
	double xoffset = 0.;
	double yoffset = 0.;
	if ( ( height() - 4 ) / m_document->height() > ( width() - 4 ) / m_document->width() )
	{
		scaleFactor = ( width() - 4 ) / m_document->width();
		yoffset = ( ( height() - 4 ) / scaleFactor - m_document->height() ) / 2;
	}
	else
	{
		scaleFactor = ( height() - 4 ) / m_document->height();
		xoffset = ( ( width() - 4 ) / scaleFactor - m_document->width() ) / 2;
	}
	// TODO: needs porting
	// QRectF rect = m_view->canvasWidget()->boundingBox();
QRectF rect = QRectF();

	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );
	if( event->type() == QEvent::MouseButtonPress )
	{
		m_firstPoint.setX( mouseEvent->pos().x() );
		m_firstPoint.setY( mouseEvent->pos().y() );
		m_lastPoint = m_firstPoint;
		QPointF p3( m_firstPoint.x() / scaleFactor - xoffset,
					( height() - m_firstPoint.y() ) / scaleFactor - yoffset );
		m_dragging = rect.contains( p3 );
	}
	else if( event->type() == QEvent::MouseButtonRelease )
	{
		if( m_dragging )
		{
			m_lastPoint.setX( mouseEvent->pos().x() );
			m_lastPoint.setY( mouseEvent->pos().y() );
			double dx = m_lastPoint.x() - m_firstPoint.x();
			double dy = m_lastPoint.y() - m_firstPoint.y();
			scaleFactor /= m_view->zoom();
	// TODO: needs porting
			// m_view->canvasWidget()->scrollContentsBy( int( dx / scaleFactor ), int( dy / scaleFactor ) );
			m_firstPoint = m_lastPoint;
			m_dragging = false;
			update();
		}
	}
	else if( event->type() == QEvent::MouseMove )
	{
		if( m_dragging )
		{
			m_lastPoint.setX( mouseEvent->pos().x() );
			m_lastPoint.setY( mouseEvent->pos().y() );
			update();
			/*double dx = m_lastPoint.x() - m_firstPoint.x();
			double dy = m_lastPoint.y() - m_firstPoint.y();
			scaleFactor /= m_view->zoom();
			m_view->canvasWidget()->scrollBy( int( dx / scaleFactor ), int( dy / scaleFactor ) );
			m_firstPoint = m_lastPoint;*/
		}
		else
		{
			QPointF p3( mouseEvent->pos().x() / scaleFactor - xoffset,
						( height() - mouseEvent->pos().y() ) / scaleFactor - yoffset );
			setCursor( rect.contains( p3 ) ? Qt::SizeAllCursor : QCursor( Qt::ArrowCursor ) );
		}
	}

	return QWidget::eventFilter( object, event );
}

void
VDocumentPreview::paintEvent( QPaintEvent* )
{
	// TODO : use NotROP, otherwise too slow
	QPixmap pixmap( width(), height() );
	double xoffset = 0.;
	double yoffset = 0.;
	double scaleFactor;
	if ( ( height() - 4 ) / m_document->height() > ( width() - 4 ) / m_document->width() )
	{
		scaleFactor = ( width() - 4 ) / m_document->width();
		yoffset = ( ( height() - 4 ) / scaleFactor - m_document->height() ) / 2;
	}
	else
	{
		scaleFactor = ( height() - 4 ) / m_document->height();
		xoffset = ( ( width() - 4 ) / scaleFactor - m_document->width() ) / 2;
	}
	xoffset += 2 / scaleFactor;
	yoffset += 2 / scaleFactor;
	if( !m_docpixmap || m_docpixmap->width() != width() || m_docpixmap->height() != height() )
	{
		delete m_docpixmap;
		m_docpixmap = new QPixmap( width(), height() );
		VQPainter p( m_docpixmap, width(), height() );
		p.clear( QColor( 195, 194, 193 ) );
		p.setMatrix( QMatrix( 1, 0, 0, -1, xoffset * scaleFactor, height() - yoffset * scaleFactor ) );
		p.setZoomFactor( scaleFactor );
		QRectF rect( -xoffset, -yoffset, m_document->width() + xoffset, m_document->height() + yoffset );
		// draw doc outline
		VColor c( Qt::black );
		VStroke stroke( c, 0L, 1.0 / scaleFactor );
		p.setPen( stroke );
		p.setBrush( Qt::white );
		p.drawRect( QRectF( 2, 2, m_document->width() - 2, m_document->height() - 2 ) );
		m_document->draw( &p, &rect );
		p.end();
	}
	QPainter pmp(&pixmap);
	pmp.drawPixmap( QPoint(0, 0), *m_docpixmap, QRect(0, 0, width(), height()) );

	// draw viewport rect
	{
		QPainter p( &pixmap );
		p.setMatrix( QMatrix( scaleFactor, 0, 0, -scaleFactor, xoffset * scaleFactor, height() - yoffset * scaleFactor ) );
		p.setPen( QColor( "red" ) );
		double dx = ( m_lastPoint.x() - m_firstPoint.x() ) * m_view->zoom();
		double dy = ( m_lastPoint.y() - m_firstPoint.y() ) * m_view->zoom();
		QPointF p1( dx / scaleFactor, dy / scaleFactor );
	// TODO: needs porting
		// p1 = m_view->canvasWidget()->toContents( p1 );
		QPointF p2( dx / scaleFactor + m_view->canvasWidget()->width(), dy / scaleFactor + m_view->canvasWidget()->height() );
	// TODO: needs porting
		// p2 = m_view->canvasWidget()->toContents( p2 );
		p.drawRect( int( p1.x() ), int( p1.y() ), int( p2.x() - p1.x() ), int( p2.y() - p1.y() ) );
	}

	QPainter pw( &pixmap );
	pw.setPen( palette().light().color() );
	pw.drawLine( 1, 1, 1, height() - 2 );
	pw.drawLine( 1, 1, width() - 2, 1 );
	pw.drawLine( width() - 1, height() - 1, 0, height() - 1 );
	pw.drawLine( width() - 1, height() - 1, width() - 1, 0 );
	pw.setPen( palette().dark().color() );
	pw.drawLine( 0, 0, width() - 1, 0 );
	pw.drawLine( 0, 0, 0, height() - 1 );
	pw.drawLine( width() - 2, height() - 2, width() - 2, 1 );
	pw.drawLine( width() - 2, height() - 2, 1, height() - 2 );
	pw.end();
	QPainter pmp2(this);
	pmp2.drawPixmap( QPoint(0, 0), pixmap, QRect(0, 0, width(), height()) );
} // VDocumentPreview::paintEvent

VDocumentTab::VDocumentTab( KarbonView* view, QWidget* parent )
		: QWidget( parent ), m_view( view )
{
	setObjectName("DocumentTab");

	QFrame* frame;
	QGridLayout* layout = new QGridLayout;
	layout->setMargin( 3 );
	layout->setSpacing( 2 );
	layout->addWidget( m_documentPreview = new VDocumentPreview( m_view, this ), 0, 7, 2, 2 );
	layout->addWidget( new QLabel( i18n( "Width:" ), this ), 0, 0 );
	layout->addWidget( new QLabel( i18n( "Height:" ), this ), 1, 0 );
	layout->addWidget( frame = new QFrame( this ), 2, 2, 0, 1 );
	frame->setFrameShape( QFrame::HLine );
	layout->addWidget( new QLabel( i18n( "Layers:" ), this ), 3, 0 );
	layout->addWidget( new QLabel( i18n( "Format:" ), this ), 4, 0 );
	layout->addWidget( frame = new QFrame( this ), 5, 5, 0, 1 );
	frame->setFrameShape( QFrame::HLine );
	//layout->addMultiCellWidget( new QLabel( i18n( "Zoom factor:" ), this ), 6, 6, 0, 1 );
	layout->addWidget( m_width = new QLabel( this ), 0, 1 );
	layout->addWidget( m_height = new QLabel( this ), 1, 1 );
	layout->addWidget( m_layers = new QLabel( this ), 3, 1 );
	layout->addWidget( m_format = new QLabel( this ), 4, 1 );
	layout->setRowStretch( 7, 1 );
	layout->setColumnStretch( 0, 0 );
	layout->setColumnStretch( 1, 0 );
	layout->setColumnStretch( 2, 2 );
	//layout->addWidget(

	m_width->setAlignment( Qt::AlignRight );
	m_height->setAlignment( Qt::AlignRight );
	m_layers->setAlignment( Qt::AlignRight );
	m_format->setAlignment( Qt::AlignRight );

	/*
	connect( view->part()->commandHistory(), SIGNAL( commandAdded( VCommand* ) ), this, SLOT( slotCommandAdded( VCommand* ) ) );
	connect( view->part()->commandHistory(), SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
	*/
	connect( view, SIGNAL( pageLayoutChanged() ), this, SLOT( slotCommandExecuted() ) );
	connect( view->canvasWidget(), SIGNAL( viewportChanged() ), this, SLOT( slotViewportChanged() ) );
	setLayout(layout);

	updateDocumentInfo();
} // VDocumentTab::VDocumentTab

VDocumentTab::~VDocumentTab()
{
} // VDocumentTab::~VDocumentTab

void
VDocumentTab::updateDocumentInfo()
{
	m_width->setText( KoUnit::toUserStringValue( m_view->part()->document().width(), m_view->part()->unit() ) + m_view->part()->unitName() );
	m_height->setText( KoUnit::toUserStringValue( m_view->part()->document().height(), m_view->part()->unit() ) + m_view->part()->unitName() );
	m_layers->setText( QString::number( m_view->part()->document().layers().count() ) );
} // VDocumentTab::updateDocumentInfo

void
VDocumentTab::slotCommandAdded( VCommand * )
{
	m_documentPreview->reset();
	m_documentPreview->update();
}

void
VDocumentTab::slotZoomChanged( double )
{
	m_documentPreview->update();
}

void
VDocumentTab::slotViewportChanged()
{
	m_documentPreview->update();
	updateDocumentInfo();
}

void
VDocumentTab::slotCommandExecuted()
{
	m_documentPreview->reset();
	m_documentPreview->update();
}

#include "vdocumentdocker.moc"
