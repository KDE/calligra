/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qdom.h>

#include "vglobal.h"
#include "vsegment.h"
#include "vsegmentlist.h"

#include <kdebug.h>


// Calculate height of p above line AB.
static double
height(
	const KoPoint& a,
	const KoPoint& p,
	const KoPoint& b )
{
	// calculate determinant of AP and AB to obtain projection of vector AP to
	// the orthogonal vector of AB:
	const double det =
		p.x() * a.y() + b.x() * p.y() - p.x() * b.y() -
		a.x() * p.y() + a.x() * b.y() - b.x() * a.y();

	// calculate norm = length(AB):
	const double norm = sqrt(
		( b.x() - a.x() ) * ( b.x() - a.x() ) +
		( b.y() - a.y() ) * ( b.y() - a.y() ) );

	// if norm is very small, simply use distance AP:
	if( norm < VGlobal::verySmallNumber )
		return
			sqrt(
				( p.x() - a.x() ) * ( p.x() - a.x() ) +
				( p.y() - a.y() ) * ( p.y() - a.y() ) );

	// normalize:
	return QABS( det ) / norm;
}


VSegment::VSegment()
{
	m_prev = 0L;
	m_next = 0L;

	m_type = segment_begin;
	m_smooth = false;
}

VSegment::VSegment( const VSegment& segment )
{
	m_prev = 0L;
	m_next = 0L;

	m_point[0] = segment.m_point[0];
	m_point[1] = segment.m_point[1];
	m_point[2] = segment.m_point[2];

	m_type = segment.m_type;
	m_smooth = segment.m_smooth;
}

bool
VSegment::isFlat( double flatness ) const
{
	if(
		!m_prev ||
		m_type == segment_begin ||
		m_type == segment_line ||
		m_type == segment_end )
	{
		return true;
	}

	if( m_type == segment_curve )
		return
			height( m_prev->m_point[2], m_point[0], m_point[2] )
				< flatness &&
			height( m_prev->m_point[2], m_point[1], m_point[2] )
				< flatness;
	else if( m_type == segment_curve1 )
		return
			height( m_prev->m_point[2], m_point[1], m_point[2] )
				< flatness;
	else if( m_type == segment_curve2 )
		return
			height( m_prev->m_point[2], m_point[0], m_point[2] )
				< flatness;

	return false;
}

double
VSegment::length() const
{
	if( !m_prev )
		return 0.0;

// TODO: length for all other segment types:
	if( m_type == segment_line )
		return
			sqrt(
				( m_point[2].x() - m_prev->m_point[2].x() ) *
				( m_point[2].x() - m_prev->m_point[2].x() ) +
				( m_point[2].y() - m_prev->m_point[2].y() ) *
				( m_point[2].y() - m_prev->m_point[2].y() ) );
	else
		return 0.0;
}

KoRect
VSegment::boundingBox() const
{
	// initialize with p3:
	KoRect rect( m_point[2], m_point[2] );

	if( m_prev )
	{
		if( m_prev->m_point[2].x() < rect.left() )
			rect.setLeft( m_prev->m_point[2].x() );
		if( m_prev->m_point[2].x() > rect.right() )
			rect.setRight( m_prev->m_point[2].x() );
		if( m_prev->m_point[2].y() < rect.top() )
			rect.setTop( m_prev->m_point[2].y() );
		if( m_prev->m_point[2].y() > rect.bottom() )
			rect.setBottom( m_prev->m_point[2].y() );
	}

	if(
		m_type == segment_curve1 ||
		m_type == segment_curve )
	{
		if( m_point[1].x() < rect.left() )
			rect.setLeft( m_point[1].x() );
		if( m_point[1].x() > rect.right() )
			rect.setRight( m_point[1].x() );
		if( m_point[1].y() < rect.top() )
			rect.setTop( m_point[1].y() );
		if( m_point[1].y() > rect.bottom() )
			rect.setBottom( m_point[1].y() );
	}

	if(
		m_type == segment_curve2 ||
		m_type == segment_curve )
	{
		if( m_point[0].x() < rect.left() )
			rect.setLeft( m_point[0].x() );
		if( m_point[0].x() > rect.right() )
			rect.setRight( m_point[0].x() );
		if( m_point[0].y() < rect.top() )
			rect.setTop( m_point[0].y() );
		if( m_point[0].y() > rect.bottom() )
			rect.setBottom( m_point[0].y() );
	}

	return rect;
}

VSegment*
VSegment::splitAt( double t )
{
	if( !m_prev || m_type == segment_begin )
		return 0L;

	VSegment* segment = new VSegment();

	// lines are easy: no need to change the current segment:
	if(
		m_type == segment_line ||
		m_type == segment_end )
	{
		segment->m_point[2] =
			m_prev->m_point[2] +
			( m_point[2] - m_prev->m_point[2] ) * t;

		segment->m_type = segment_line;
		return segment;
	}

	// these references make our life a bit easier:
	KoPoint& p0 = m_prev->m_point[2];
	KoPoint& p3 = m_point[2];
	KoPoint& p1 = m_type == segment_curve1 ? p0 : m_point[0];
	KoPoint& p2 = m_type == segment_curve2 ? p3 : m_point[1];

	// calculate the 2 new beziers:
	segment->m_point[0] = p0 + ( p1 - p0 ) * t;
	segment->m_point[1] = p1 + ( p2 - p1 ) * t;

	p2 = p2 + ( p3 - p2 ) * t;
	p1 = segment->m_point[1] + ( p2 - segment->m_point[1] ) * t;

	segment->m_point[1] =
		segment->m_point[0] + ( segment->m_point[1] - segment->m_point[0] ) * t;
	segment->m_point[2] =
		segment->m_point[1] + ( p1 - segment->m_point[1] ) * t;

	// and finally set the new segment type properly:
	if( m_type == segment_curve1 )
	{
		segment->m_type = segment_curve1;
		m_type = segment_curve;
	}
	else
		segment->m_type = segment_curve;

	return segment;
}

void
VSegment::convertToCurve( double t )
{
	if(
		!m_prev ||
		m_type == segment_begin ||
		m_type == segment_end )
	{
		return;
	}

	if( m_type == segment_line )
	{
		m_point[0] =
			m_prev->m_point[2] +
			( m_point[2] - m_prev->m_point[2] ) * t;
		m_point[1] =
			m_prev->m_point[2] +
			( m_point[2] - m_prev->m_point[2] ) * ( 1.0 - t );
	}
	else if( m_type == segment_curve1 )
		m_point[0] = m_prev->m_point[2];
	else if( m_type == segment_curve2 )
		m_point[1] = m_point[2];

	m_type = segment_curve;
}

bool
VSegment::linesIntersect(
	const KoPoint& a0,
	const KoPoint& a1,
	const KoPoint& b0,
	const KoPoint& b1 )
{
	const KoPoint delta_a = a1 - a0;
	const double det_a = a1.x() * a0.y() - a1.y() * a0.x();

	const double r_b0 = delta_a.y() * b0.x() - delta_a.x() * b0.y() + det_a;
	const double r_b1 = delta_a.y() * b1.x() - delta_a.x() * b1.y() + det_a;

	if( r_b0 != 0.0 && r_b1 != 0.0 && r_b0 * r_b1 > 0.0 )
		return false;

	const KoPoint delta_b = b1 - b0;
	const double det_b = b1.x() * b0.y() - b1.y() * b0.x();

	const double r_a0 = delta_b.y() * a0.x() - delta_b.x() * a0.y() + det_b;
	const double r_a1 = delta_b.y() * a1.x() - delta_b.x() * a1.y() + det_b;

	if( r_a0 != 0.0 && r_a1 != 0.0 && r_a0 * r_a1 > 0.0 )
		return false;

	return true;
}

void
VSegment::save( QDomElement& element ) const
{
	QDomElement me;

	if( m_type == segment_curve )
	{
		me = element.ownerDocument().createElement( "CURVE" );
		me.setAttribute( "x1", m_point[0].x() );
		me.setAttribute( "y1", m_point[0].y() );
		me.setAttribute( "x2", m_point[1].x() );
		me.setAttribute( "y2", m_point[1].y() );
		me.setAttribute( "x3", m_point[2].x() );
		me.setAttribute( "y3", m_point[2].y() );
	}
	else if( m_type == segment_curve1 )
	{
		me = element.ownerDocument().createElement( "CURVE1" );
		me.setAttribute( "x2", m_point[1].x() );
		me.setAttribute( "y2", m_point[1].y() );
		me.setAttribute( "x3", m_point[2].x() );
		me.setAttribute( "y3", m_point[2].y() );
	}
	else if( m_type == segment_curve2 )
	{
		me = element.ownerDocument().createElement( "CURVE2" );
		me.setAttribute( "x1", m_point[0].x() );
		me.setAttribute( "y1", m_point[0].y() );
		me.setAttribute( "x3", m_point[2].x() );
		me.setAttribute( "y3", m_point[2].y() );
	}
	else if( m_type == segment_line )
	{
		me = element.ownerDocument().createElement( "LINE" );
		me.setAttribute( "x", m_point[2].x() );
		me.setAttribute( "y", m_point[2].y() );
	}
	else if( m_type == segment_begin )
	{
		me = element.ownerDocument().createElement( "MOVE" );
		me.setAttribute( "x", m_point[2].x() );
		me.setAttribute( "y", m_point[2].y() );
	}
	else
		return;

	element.appendChild( me );

}

void
VSegment::load( const QDomElement& element )
{
	if( element.tagName() == "CURVE" )
	{
		m_type = segment_curve;
		m_point[0].setX( element.attribute( "x1" ).toDouble() );
		m_point[0].setY( element.attribute( "y1" ).toDouble() );
		m_point[1].setX( element.attribute( "x2" ).toDouble() );
		m_point[1].setY( element.attribute( "y2" ).toDouble() );
		m_point[2].setX( element.attribute( "x3" ).toDouble() );
		m_point[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "CURVE1" )
	{
		m_type = segment_curve1;
		m_point[1].setX( element.attribute( "x2" ).toDouble() );
		m_point[1].setY( element.attribute( "y2" ).toDouble() );
		m_point[2].setX( element.attribute( "x3" ).toDouble() );
		m_point[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "CURVE2" )
	{
		m_type = segment_curve2;
		m_point[0].setX( element.attribute( "x1" ).toDouble() );
		m_point[0].setY( element.attribute( "y1" ).toDouble() );
		m_point[2].setX( element.attribute( "x3" ).toDouble() );
		m_point[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "LINE" )
	{
		m_type = segment_line;
		m_point[2].setX( element.attribute( "x" ).toDouble() );
		m_point[2].setY( element.attribute( "y" ).toDouble() );
	}
	else if( element.tagName() == "MOVE" )
	{
		m_type = segment_begin;
		m_point[2].setX( element.attribute( "x" ).toDouble() );
		m_point[2].setY( element.attribute( "y" ).toDouble() );
	}
}
