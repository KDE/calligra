/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainter.h"
#include <qpointarray.h>
#include <qcolor.h>
#include <qpen.h>

#include "vpath_stroke.h"

#include <kdebug.h>

VPathStroke::VPathStroke( const VStroke& stroke )
	: VSegmentListTraverser(), m_stroke( stroke )
{
}

void
VPathStroke::draw( VPainter *painter, const double zoomFactor, const VSegmentList& list,
	bool plain )
{
	m_zoomFactor = zoomFactor;
	m_painter = painter;

	traverse( list );

	if( !plain )
	{
		/*int r;
		int g;
		int b;
		
		color().pseudoValues( r, g, b );

		QPen pen(
			QColor( r, g, b ),
			qRound( zoomFactor * lineWidth() ) );*/

		/*if( lineCap() == cap_butt )
			pen.setCapStyle( Qt::FlatCap );
		else if( lineCap() == cap_round )
			pen.setCapStyle( Qt::RoundCap );
		else if( lineCap() == cap_square )
			pen.setCapStyle( Qt::SquareCap );*/

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( m_stroke );
		//painter->setPen( pen );
	}
	else
	{
		painter->setRasterOp( Qt::XorROP );
		painter->setPen( Qt::yellow );
	}

	painter->setBrush( Qt::NoBrush );
	painter->strokePath();
}

bool
VPathStroke::begin( const KoPoint& p )
{
	m_painter->moveTo( p );

	/*m_pa.resize( 1 );
	m_pa.setPoint(
		0,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );*/

	return true;
}

bool
VPathStroke::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	m_painter->curveTo( p1, p2, p3 );

	/*QPointArray pa( 4 );
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

	setPreviousPoint( p3 );*/

	return true;
}

bool
VPathStroke::lineTo( const KoPoint& p )
{
	m_painter->lineTo( p );

	/*m_pa.resize( m_pa.size() + 1 );
	m_pa.setPoint( m_pa.size() - 1,
		qRound( m_zoomFactor * p.x() ),
		qRound( m_zoomFactor * p.y() ) );

	setPreviousPoint( p );*/

	return true;
}

