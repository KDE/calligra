/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpath_bounding.h"
#include "vsegmenttools.h"

#include <kdebug.h>

VPathBounding::VPathBounding()
	: VSegmentListTraverser()
{
}

void
VPathBounding::calculate( QRect& rect, const double zoomFactor, const VSegmentList& list )
{
	m_zoomFactor = zoomFactor;
	m_rect = &rect;
	m_quick = true;
	traverse( list );
}

bool
VPathBounding::intersects( const QRect& rect, const double zoomFactor, const VSegmentList& list )
{
	m_zoomFactor = zoomFactor;
	m_quick = false;
	traverse( list );

	if( !m_pa.boundingRect().intersects( rect ) )
		return false;

	// check for line intersections:
	for( uint i = 1; i < m_pa.count(); ++i )
	{
		if( VSegmentTools::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.left(), rect.top() ),
			KoPoint( rect.right(), rect.top() ) ) )
				return true;
		if( VSegmentTools::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.right(), rect.top() ),
			KoPoint( rect.right(), rect.bottom() ) ) )
				return true;
		if( VSegmentTools::linesIntersect(
			KoPoint( m_pa.point( i - 1 ).x(), m_pa.point( i - 1 ).y() ),
			KoPoint( m_pa.point( i ).x(), m_pa.point( i ).y() ),
			KoPoint( rect.right(), rect.bottom() ),
			KoPoint( rect.left(), rect.bottom() ) ) )
				return true;
		if( VSegmentTools::linesIntersect(
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
	if( m_quick )
	{
		m_rect->setCoords( p.x(), p.y(), p.x(), p.y() );
	}
	else
	{
		m_pa.resize( 1 );
		m_pa.setPoint(
			0,
			qRound( m_zoomFactor * p.x() ),
			qRound( m_zoomFactor * p.y() ) );
	}

	setPreviousPoint( p );

	return true;
}

bool
VPathBounding::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	if( m_quick )
	{
		const int x1 = qRound( m_zoomFactor * p1.x() );
		const int y1 = qRound( m_zoomFactor * p1.y() );
		const int x2 = qRound( m_zoomFactor * p2.x() );
		const int y2 = qRound( m_zoomFactor * p2.y() );
		const int x3 = qRound( m_zoomFactor * p3.x() );
		const int y3 = qRound( m_zoomFactor * p3.y() );

		if( x1 < m_rect->left() )
			m_rect->setLeft( x1 );
		if( x1 > m_rect->right() )
			m_rect->setRight( x1 );
		if( y1 < m_rect->top() )
			m_rect->setTop( y1 );
		if( y1 > m_rect->bottom() )
			m_rect->setBottom( y1 );
		if( x2 < m_rect->left() )
			m_rect->setLeft( x2 );
		if( x2 > m_rect->right() )
			m_rect->setRight( x2 );
		if( y2 < m_rect->top() )
			m_rect->setTop( y2 );
		if( y2 > m_rect->bottom() )
			m_rect->setBottom( y2 );
		if( x3 < m_rect->left() )
			m_rect->setLeft( x3 );
		if( x3 > m_rect->right() )
			m_rect->setRight( x3 );
		if( y3 < m_rect->top() )
			m_rect->setTop( y3 );
		if( y3 > m_rect->bottom() )
			m_rect->setBottom( y3 );
	}
	else
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
	}

	setPreviousPoint( p3 );

	return true;
}

bool
VPathBounding::lineTo( const KoPoint& p )
{
	if( m_quick )
	{
		const int x = qRound( m_zoomFactor * p.x() );
		const int y = qRound( m_zoomFactor * p.y() );

		if( x < m_rect->left() )
			m_rect->setLeft( x );
		if( x > m_rect->right() )
			m_rect->setRight( x );
		if( y < m_rect->top() )
			m_rect->setTop( y );
		if( y > m_rect->bottom() )
			m_rect->setBottom( y );
	}
	else
	{
		m_pa.resize( m_pa.size() + 1 );
		m_pa.setPoint(
			m_pa.size() - 1,
			qRound( m_zoomFactor * p.x() ),
			qRound( m_zoomFactor * p.y() ) );
	}

	setPreviousPoint( p );

	return true;
}
