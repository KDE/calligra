/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <qpointarray.h>

#include "vcontour.h"

VContour::VContour()
	: VSegmentListTraverser()
{
}

void
VContour::draw( QPainter& painter, const double zoomFactor, const VSegmentList& list )
{
	m_painter = &painter;
	m_zoomFactor = zoomFactor;

	m_painter->save();
	VSegment::traverse( list, *this );
	m_painter->restore();
}

void
VContour::begin( const KoPoint& p )
{
	m_painter->moveTo(
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
	m_painter->drawPolyline( pa.cubicBezier() );

	m_painter->moveTo(
		qRound( m_zoomFactor * p3.x() ),
		qRound( m_zoomFactor * p3.y() ) );

	setPreviousPoint( p3 );
}

void
VContour::lineTo( const KoPoint& p )
{
	m_painter->lineTo(
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );
}
