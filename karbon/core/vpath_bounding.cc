/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpath_bounding.h"

#include <kdebug.h>

VPathBounding::VPathBounding()
	: VSegmentListTraverser()
{
}

bool
VPathBounding::intersects( const KoRect& rect, const double zoomFactor, const VSegmentList& list )
{
	m_zoomFactor = zoomFactor;
	traverse( list );

	if( !m_pa.boundingRect().intersects( rect.toQRect() ) )
		return false;

	// check for line intersections:
	for( uint i = 1; i < m_pa.count(); ++i )
	{
		if( VSegment::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.left(), rect.top() ),
			KoPoint( rect.right(), rect.top() ) ) )
				return true;
		if( VSegment::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.right(), rect.top() ),
			KoPoint( rect.right(), rect.bottom() ) ) )
				return true;
		if( VSegment::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.right(), rect.bottom() ),
			KoPoint( rect.left(), rect.bottom() ) ) )
				return true;
		if( VSegment::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.left(), rect.bottom() ),
			KoPoint( rect.left(), rect.top() ) ) )
				return true;
	}

	// check if rect is completely inside m_pa:
	for( uint i = 0; i < m_pa.count(); ++i )
	{
		if( rect.contains( m_pa.point( i ) ) )
			return true;
	}

	return false;
}

bool
VPathBounding::begin( const KoPoint& p )
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
VPathBounding::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
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
VPathBounding::lineTo( const KoPoint& p )
{
	m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint(
		m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );

	return true;
}
