/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <qpointarray.h>

#include "vcontour.h"

VContour::VContour( const double width, const VLineCap cap, const VLineJoin join )
	: VSegmentListTraverser(), m_lineWidth( width ), m_lineCap( cap ), m_lineJoin( join )
{
}

void
VContour::draw( QPainter& painter, const double zoomFactor, const VSegmentList& list,
	bool plain )
{
	m_zoomFactor = zoomFactor;

	m_pa.resize( 0 );
	VSegment::traverse( list, *this );

	if( !plain )
	{
		int r;
		int g;
		int b;
		
		m_color.values( VColor::rgb, &r, &g, &b );

		QPen pen(
			QColor( r, g, b ),
			qRound( zoomFactor * m_lineWidth ) );

		if( m_lineCap == cap_butt )
			pen.setCapStyle( Qt::FlatCap );
		else if( m_lineCap == cap_round )
			pen.setCapStyle( Qt::RoundCap );
		else if( m_lineCap == cap_square )
			pen.setCapStyle( Qt::SquareCap );

		painter.setPen( pen );
	}

	painter.setBrush( Qt::NoBrush );
	painter.drawPolyline( m_pa );
}

void
VContour::begin( const KoPoint& p )
{
	m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint( m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );
}

void
VContour::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	QPointArray pa( 4 );
	pa.setPoint( 0,
		qRound( m_zoomFactor * previousPoint().x() ),
		qRound( m_zoomFactor * previousPoint().y() ) );
	pa.setPoint( 1,
		qRound( m_zoomFactor * p1.x() ),
		qRound( m_zoomFactor * p1.y() ) );
	pa.setPoint( 2,
		qRound( m_zoomFactor * p2.x() ),
		qRound( m_zoomFactor * p2.y() ) );
	pa.setPoint( 3,
		qRound( m_zoomFactor * p3.x() ),
		qRound( m_zoomFactor * p3.y() ) );

	QPointArray pa2( pa.cubicBezier() );

	m_pa.resize( m_pa.size() + pa2.size() );
	m_pa.putPoints( m_pa.size() - pa2.size(), pa2.size(), pa2 );

	setPreviousPoint( p3 );
}

void
VContour::lineTo( const KoPoint& p )
{
	m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint( m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );
}
