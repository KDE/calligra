/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <qpointarray.h>

#include "vfill.h"

VFill::VFill()
	: VSegmentListTraverser()
{
}

void
VFill::draw( QPainter& painter, const double zoomFactor, const VSegmentList& list )
{
	m_zoomFactor = zoomFactor;

	m_pa.resize( 0 );
	VSegment::traverse( list, *this );

	painter.save();
	painter.setPen( Qt::NoPen );
	painter.setBrush( QColor( 210, 210, 210 ) );
	painter.drawPolygon( m_pa );
	painter.restore();
}

void
VFill::begin( const KoPoint& p )
{
	m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint( m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );
}

void
VFill::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
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
VFill::lineTo( const KoPoint& p )
{
	m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint( m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );
}
