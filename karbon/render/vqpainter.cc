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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// qpainter wrapper

#include "vqpainter.h"
#include "vstroke.h"
#include "vcolor.h"
#include "vfill.h"

#include <qpainter.h>
#include <qpaintdevice.h>
#include <qpen.h>

#include <QPointF>
#include <kdebug.h>

VQPainter::VQPainter( QPaintDevice *target, unsigned int w, unsigned int h, bool drawNodes ) : VPainter( target, w, h ), m_painter( 0L ), m_target( target ), m_width( w ), m_height( h ), m_bDrawNodes( drawNodes )
{
	m_zoomFactor = 1;
	m_index = 0;
	m_painter = new QPainter( target );
}

VQPainter::VQPainter( unsigned char *buffer, unsigned int w, unsigned int h, bool drawNodes ) : VPainter( 0L, w, h ), m_painter( 0L ), m_width( w ), m_height( h ), m_bDrawNodes( drawNodes )
{
	m_buffer = buffer;
	m_zoomFactor = 1;
	m_index = 0;
	// TODO: load the buffer in the painter
	//m_painter = new QPainter( QPixmap(m_buffer) );
	// for now use:
	m_painter = new QPainter;
}

VQPainter::~VQPainter()
{
	delete m_painter;
}

void
VQPainter::resize( unsigned int w, unsigned int h )
{
	m_width = w;
	m_height = h;
}

void
VQPainter::blit( const QRectF & )
{
	end();
}

void
VQPainter::clear( const QColor &c )
{
	m_painter->setBackground(QBrush(c, Qt::SolidPattern));
	m_painter->eraseRect( 0, 0, m_width, m_height );
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

const QMatrix
VQPainter::worldMatrix()
{
	return m_painter->matrix();
}

void
VQPainter::setMatrix( const QMatrix& mat )
{
	m_painter->setMatrix( mat );
}

void
VQPainter::setZoomFactor( double zoomFactor )
{
	m_zoomFactor = zoomFactor;
	/*QMatrix mat;
	mat.scale( zoomFactor, zoomFactor );
	m_painter->setMatrix( mat );*/
}

void 
VQPainter::moveTo( const QPointF &p )
{
	m_pa.moveTo(p * m_zoomFactor);
}

void 
VQPainter::lineTo( const QPointF &p )
{
	m_pa.lineTo(p * m_zoomFactor);
}

void
VQPainter::curveTo( const QPointF &p1, const QPointF &p2, const QPointF &p3 )
{
	m_pa.cubicTo(p1 * m_zoomFactor, p2 * m_zoomFactor, p3 * m_zoomFactor);
}

void
VQPainter::newPath()
{
	m_index = 0;
}

void
VQPainter::fillPath()
{
	m_painter->fillPath(m_pa, m_painter->brush());
}

void
VQPainter::strokePath()
{
	m_painter->strokePath(m_pa, m_painter->pen());
}

void
VQPainter::setPen( const VStroke &stroke )
{
	QPen pen;

	// color + linewidth
	pen.setColor( stroke.color() );
	pen.setWidth( static_cast<int>(stroke.lineWidth()) );

	// caps
	if( stroke.lineCap() == VStroke::capButt )
		pen.setCapStyle( Qt::FlatCap );
	else if( stroke.lineCap() == VStroke::capRound )
		pen.setCapStyle( Qt::RoundCap );
	else if( stroke.lineCap() == VStroke::capSquare )
		pen.setCapStyle( Qt::SquareCap );

	m_painter->setPen( pen );
}

void
VQPainter::setBrush( const VFill &fill )
{
	switch( fill.type() )
	{
		case VFill::none:
			m_painter->setBrush( Qt::NoBrush );
		break;
		case VFill::solid:
			m_painter->setBrush( QBrush( fill.color(), Qt::SolidPattern ) );
		break;
		case VFill::grad:
			// gradients are nor supported by qpainter
			m_painter->setBrush( Qt::NoBrush );
		break;
		case VFill::patt:
			// pixmap brushes not supported for printing
			m_painter->setBrush( QBrush( fill.color(), fill.pattern().pixmap() ) );
		break;
		default:
		break;
	}
}

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

/* TODO: Needs to be ported to Qt4
void
VQPainter::setRasterOp( Qt::RasterOp r )
{
	m_painter->setRasterOp( r );
}
*/

void
VQPainter::drawNode( const QPointF &p, int width )
{
	m_painter->drawRect( QRect( int( p.x() * m_zoomFactor ) - width, int( p.y() * m_zoomFactor ) - width,
	                            2 * width + 1,  2 * width + 1 ) );
}

void
VQPainter::drawRect( const QRectF &rect )
{
	m_painter->drawRect( QRect( int( rect.x() ), int( rect.y() ), int( rect.width() ),  int( rect.height() ) ) );
}

void
VQPainter::drawRect( double, double, double, double )
{
//TODO: Implement
}

void
VQPainter::drawImage( const QImage &image, const QMatrix &affine )
{
	QMatrix matrix = m_painter->matrix();
	
	double m11 = affine.m11() * matrix.m11() * m_zoomFactor + affine.m12() * matrix.m21();
	double m12 = (affine.m11() * matrix.m12() + affine.m12() * matrix.m22() ) * m_zoomFactor;
	double m21 = (affine.m21() * matrix.m11() + affine.m22() * matrix.m21() ) * m_zoomFactor;
	double m22 = affine.m22() * matrix.m22() * m_zoomFactor + affine.m21() * matrix.m12();
	double dx = matrix.dx() + affine.dx() * m_zoomFactor;
	double dy = matrix.dy() - affine.dy() * m_zoomFactor;
	
	QMatrix world( m11, m12, m21, m22, dx, dy );
	
	m_painter->setMatrix( world );
	
	m_painter->drawImage( QPoint( 0, 0 ), image );
	// restore old world matrix
	m_painter->setMatrix( matrix );
}
