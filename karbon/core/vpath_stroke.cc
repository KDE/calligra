/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <qpointarray.h>

#include "vpath_stroke.h"

#include <kdebug.h>

VPathStroke::VPathStroke( const double width, const VLineCap cap, const VLineJoin join )
	: VStroke( width, cap, join ), VSegmentListTraverser()
{
}

void
VPathStroke::draw( QPainter& painter, const double zoomFactor, const VSegmentList& list,
	bool plain )
{
	m_zoomFactor = zoomFactor;

	traverse( list );

	if( !plain )
	{
		int r;
		int g;
		int b;
		
		color().values( VColor::rgb, &r, &g, &b );

		QPen pen(
			QColor( r, g, b ),
			qRound( zoomFactor * lineWidth() ) );

		if( lineCap() == cap_butt )
			pen.setCapStyle( Qt::FlatCap );
		else if( lineCap() == cap_round )
			pen.setCapStyle( Qt::RoundCap );
		else if( lineCap() == cap_square )
			pen.setCapStyle( Qt::SquareCap );

		painter.setRasterOp( Qt::CopyROP );
		painter.setPen( pen );
	}
	else
	{
		painter.setRasterOp( Qt::XorROP );
		painter.setPen( Qt::yellow );
	}

	painter.setBrush( Qt::NoBrush );
	painter.drawPolyline( m_pa );
}

bool
VPathStroke::begin( const KoPoint& p,
	const VSegment& segment )
{
	m_pa.resize( 1 );
	m_pa.setPoint(
		0,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );

	return true;
}

bool
VPathStroke::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3,
	const VSegment& segment )
{
	QPointArray pa( 4 );
	pa.setPoint(
		0,
		qRound( m_zoomFactor * previousPoint().x() ),
		qRound( m_zoomFactor * previousPoint().y() ) );
	pa.setPoint(
		1,
		qRound( m_zoomFactor * p1.x() ),
		qRound( m_zoomFactor * p1.y() ) );
	pa.setPoint(
		2,
		qRound( m_zoomFactor * p2.x() ),
		qRound( m_zoomFactor * p2.y() ) );
	pa.setPoint(
		3,
		qRound( m_zoomFactor * p3.x() ),
		qRound( m_zoomFactor * p3.y() ) );

	QPointArray pa2( pa.cubicBezier() );

	m_pa.resize( m_pa.size() + pa2.size() );
	m_pa.putPoints( m_pa.size() - pa2.size(), pa2.size(), pa2 );

	setPreviousPoint( p3 );

	return true;
}

bool
VPathStroke::lineTo( const KoPoint& p,
	const VSegment& segment )
{
	m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint( m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );

	return true;
}
