/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

// qpainter wrapper

#include "vqpainter.h"
#include "vstroke.h"
#include "vcolor.h"

#include <qpainter.h>
#include <qwidget.h>
#include <qpen.h>

VQPainter::VQPainter( QWidget *target, int w, int h ) : VPainter( target, w, h ), m_painter( 0L ), m_target( target ), m_width( w ), m_height( h )
{
	m_painter = new QPainter( target );
}

VQPainter::~VQPainter()
{
	delete m_painter;
}

void
VQPainter::resize( int w, int h )
{
	m_width = w;
	m_height = h;
}

void
VQPainter::begin()
{
	if( !m_painter->isActive() )
	{
		m_painter->begin( m_target );
		m_painter->eraseRect( 0, 0, m_width, m_height );
	}
}

void
VQPainter::end()
{
	m_painter->end();
}

void
VQPainter::setWorldMatrix( const QWMatrix &mat )
{
	//m_painter->setWorldMatrix( mat );
}

void
VQPainter::drawPolygon( const QPointArray &pa, bool winding )
{
	m_painter->drawPolygon( pa, winding );
}

void
VQPainter::drawPolyline( const QPointArray &pa )
{
	m_painter->drawPolyline( pa );
}

void
VQPainter::drawRect( double x, double y, double w, double h )
{
	m_painter->drawRect( int(x), int(y), int(w), int(h) );
}

void
VQPainter::setPen( const VStroke &stroke )
{
	QPen pen;

	// color + linewidth
	int r;
	int g;
	int b;

	stroke.color().pseudoValues( r, g, b );
	pen.setColor( QColor( r, g, b ) );
	pen.setWidth( stroke.lineWidth() );

	// caps
	if( stroke.lineCap() == VStroke::cap_butt )
		pen.setCapStyle( Qt::FlatCap );
	else if( stroke.lineCap() == VStroke::cap_round )
		pen.setCapStyle( Qt::RoundCap );
	else if( stroke.lineCap() == VStroke::cap_square )
		pen.setCapStyle( Qt::SquareCap );

	m_painter->setPen( pen );
}

// void setBrush( const VBrush & );

void
VQPainter::setPen( const QColor &c )
{
	m_painter->setPen( c );
}

void
VQPainter::setPen( Qt::PenStyle style )
{
	m_painter->setPen( style );
}

void
VQPainter::setBrush( const QBrush &b )
{
	m_painter->setBrush( b );
}

void
VQPainter::setBrush( const QColor &c )
{
	m_painter->setBrush( c );
}

void
VQPainter::setBrush( Qt::BrushStyle style )
{
	m_painter->setBrush( style );
}

void
VQPainter::save()
{
	m_painter->save();
}

void
VQPainter::restore()
{
	m_painter->restore();
}

void
VQPainter::setRasterOp( Qt::RasterOp r )
{
	m_painter->setRasterOp( r );
}

