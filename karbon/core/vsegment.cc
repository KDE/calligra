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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <math.h>

#include <qdom.h>

#include "vglobal.h"
#include "vpainter.h"
#include "vpath.h"
#include "vsegment.h"

#include <kdebug.h>


// Calculate height of p above line AB.
static double
height(
	const KoPoint& a,
	const KoPoint& p,
	const KoPoint& b )
{
	// Calculate determinant of AP and AB to obtain projection of vector AP to
	// the orthogonal vector of AB.
	const double det =
		p.x() * a.y() + b.x() * p.y() - p.x() * b.y() -
		a.x() * p.y() + a.x() * b.y() - b.x() * a.y();

	// Calculate norm = length(AB).
	const double norm = sqrt(
		( b.x() - a.x() ) * ( b.x() - a.x() ) +
		( b.y() - a.y() ) * ( b.y() - a.y() ) );

	// If norm is very small, simply use distance AP.
	if( norm < VGlobal::verySmallNumber )
		return
			sqrt(
				( p.x() - a.x() ) * ( p.x() - a.x() ) +
				( p.y() - a.y() ) * ( p.y() - a.y() ) );

	// Normalize.
	return QABS( det ) / norm;
}


VSegment::VSegment( VSegmentType type )
{
	m_prev = 0L;
	m_next = 0L;

	m_nodeSelected[0] = false;
	m_nodeSelected[1] = false;
	m_nodeSelected[2] = false;

	m_nodeEdited[0] = false;
	m_nodeEdited[1] = false;
	m_nodeEdited[2] = false;

	m_state = normal;

	m_type = type;
	m_ctrlPointFixing = none;
}

VSegment::VSegment( const VSegment& segment )
{
	// Copying the pointers m_prev/m_next has some advantages (see VSegment::length()).
	// Inserting a segment into a path overwrites these anyway.
	m_prev = segment.m_prev;
	m_next = segment.m_next;

	m_node[0] = segment.m_node[0];
	m_node[1] = segment.m_node[1];
	m_node[2] = segment.m_node[2];

	m_nodeSelected[0] = segment.m_nodeSelected[0];
	m_nodeSelected[1] = segment.m_nodeSelected[1];
	m_nodeSelected[2] = segment.m_nodeSelected[2];

	m_nodeEdited[0] = segment.m_nodeEdited[0];
	m_nodeEdited[1] = segment.m_nodeEdited[1];
	m_nodeEdited[2] = segment.m_nodeEdited[2];

	m_state = segment.m_state;
	m_type = segment.m_type;

	m_ctrlPointFixing = segment.m_ctrlPointFixing;
}

void
VSegment::draw( VPainter* painter ) const
{
	if( state() == deleted )
		return;

	if( type() == curve )
	{
		painter->curveTo( ctrlPoint1(), ctrlPoint2(), knot() );
	}
	else if( type() == line )
	{
		painter->lineTo( knot() );
	}
	else
	{
		painter->moveTo( knot() );
	}
}

bool
VSegment::isFlat( double flatness ) const
{
	if(
		!m_prev ||
		m_type == begin ||
		m_type == line )
	{
		return true;
	}

	if( m_type == curve )
		return
			height( m_prev->m_node[2], m_node[0], m_node[2] )
				< flatness &&
			height( m_prev->m_node[2], m_node[1], m_node[2] )
				< flatness;

	return false;
}

KoPoint
VSegment::point( double t ) const
{
	KoPoint p;
	pointDerivatives( t, &p );
	return p;
}

void
VSegment::pointDerivatives( double t, KoPoint* p,
	KoPoint* d1, KoPoint* d2 ) const
{
	if( !m_prev || m_type == begin )
		return;

	// Lines.
	if( m_type == line )
	{
		const KoPoint diff = m_node[2] - m_prev->m_node[2];

		if( p )
			*p = m_prev->m_node[2] + diff * t;
		if( d1 )
			*d1 = diff;
		if( d2 )
			*d2 = KoPoint( 0.0, 0.0 );

		return;
	}

	// Beziers.
	KoPoint q[4];
	q[0] = m_prev->m_node[2];
	q[1] = m_node[0];
	q[2] = m_node[1];
	q[3] = m_node[2];

	// The De Casteljau algorithm.
	for( uint j = 1; j <= 3; ++j )
	{
		for( uint i = 0; i <= 3 - j; ++i )
		{
			q[i] = ( 1.0 - t ) * q[i] + t * q[i+1];
		}

		if( j == 1 )
		{
			if( d2 )
				*d2 = 6 * ( q[2] - 2 * q[1] + q[0] );
		}
		else if( j == 2 )
		{
			if( d1 )
				*d1 = 3 * ( q[1] - q[0] );
		}
	}

	if( p )
		*p = q[0];

	return;
}

void
VSegment::pointTangentNormal( double t, KoPoint* p,
	KoPoint* tn, KoPoint* n ) const
{
	// Calculate derivative if necessary.
	KoPoint d;

	pointDerivatives( t, p, tn || n ? &d : 0L );


	// Normalize derivative.
	if( tn || n )
	{
		const double norm =
			sqrt( d.x() * d.x() + d.y() * d.y() );

		d = norm ? d * ( 1.0 / norm ) : KoPoint( 0.0, 0.0 );
	}

	// Assign tangent vector.
	if( tn )
		*tn = d;

	// Calculate normal vector.
	if( n )
	{
		// Calculate vector product of "binormal" x tangent
		// (0,0,1) x (dx,dy,0), which is simply (dy,-dx,0).
		n->setX(  d.y() );
		n->setY( -d.x() );
	}
}

KoPoint
VSegment::tangent( double t ) const
{
	KoPoint tn;
	pointTangentNormal( t, 0L, &tn );
	return tn;
}

double
VSegment::length( double t ) const
{
	if( !m_prev )
		return 0.0;

	// Length of a line.
	if( m_type == line )
	{
		return
			t * chordLength();
	}
	// Length of a bezier.
	else if( m_type == curve )
	{
		// This algortihm is based on an idea by Jens Gravesen <gravesen@mat.dth.dk>.
		// We calculate the chord length "chord"=|P0P3| and the length of the control point
		// polygon "poly"=|P0P1|+|P1P2|+|P2P3|. The approximation for the bezier length is
		// 0.5 * poly + 0.5 * chord. "poly - chord" is a measure for the error.
		// We subdivide each segment until the error is smaller than a given tolerance
		// and add up the subresults.


		// "Copy segment" splitted at t into a path.
		VPath path( 0L );
		path.moveTo( prev()->knot() );

		// Optimize a bit: most of the time we'll need the
		// length of the whole segment.
		if( t == 1.0 )
			path.append( this->clone() );
		else
		{
			VSegment* copy = this->clone();
			path.append( copy->splitAt( t ) );
			delete copy;
		}


		double chord;
		double poly;

		double length = 0.0;

		while( path.current() )
		{
			chord = path.current()->chordLength();
			poly  = path.current()->polyLength();

			if(
				poly &&
				( poly - chord ) / poly > VGlobal::lengthTolerance )
			{
				// Split at midpoint.
				path.insert(
					path.current()->splitAt( 0.5 ) );
			}
			else
			{
				length += 0.5 * poly + 0.5 * chord;
				path.next();
			}
		}

		return length;
	}
	else
		return 0.0;
}

double
VSegment::chordLength() const
{
	if( !m_prev )
		return 0.0;

	return
		sqrt(
				( m_node[2].x() - m_prev->m_node[2].x() ) *
				( m_node[2].x() - m_prev->m_node[2].x() ) +
				( m_node[2].y() - m_prev->m_node[2].y() ) *
				( m_node[2].y() - m_prev->m_node[2].y() ) );
}

double
VSegment::polyLength() const
{
	if( !m_prev )
		return 0.0;

	return
		sqrt(
				( m_node[0].x() - m_prev->m_node[2].x() ) *
				( m_node[0].x() - m_prev->m_node[2].x() )
			+
				( m_node[0].y() - m_prev->m_node[2].y() ) *
				( m_node[0].y() - m_prev->m_node[2].y() ) )
		+ sqrt(
				( m_node[1].x() - m_node[0].x() ) *
				( m_node[1].x() - m_node[0].x() )
			+
				( m_node[1].y() - m_node[0].y() ) *
				( m_node[1].y() - m_node[0].y() ) )
		+ sqrt(
				( m_node[2].x() - m_node[1].x() ) *
				( m_node[2].x() - m_node[1].x() )
			+
				( m_node[2].y() - m_node[1].y() ) *
				( m_node[2].y() - m_node[1].y() ) );
}

double
VSegment::param( double len ) const
{
	if( !m_prev || len == 0.0 )		// We divide by len below.
		return 0.0;

	// Line.
	if( m_type == line )
	{
		return
			len / chordLength();
	}
	// Bezier.
	else if( m_type == curve )
	{
		// Perform a successive interval bisection.
		double param1   = 0.0;
		double paramMid = 0.5;
		double param2   = 1.0;
		double lengthMid = length( paramMid );

		while( QABS( lengthMid - len ) / len > VGlobal::paramLengthTolerance )
		{
			if( lengthMid < len )
				param1 = paramMid;
			else
				param2 = paramMid;

			paramMid = 0.5 * ( param2 + param1 );

			lengthMid = length( paramMid );
		}
		return paramMid;
	}

	return 0.0;
}

bool
VSegment::isSmooth( const VSegment* next ) const
{
	// Return false if this segment is a "begin" or if there is no
	// next segment.
	if( type() == begin || !next )
		return false;


	// Calculate tangents.
	KoPoint t1;
	KoPoint t2;

	pointTangentNormal( 1.0, 0L, &t1 );
	next->pointTangentNormal( 0.0, 0L, &t2 );


	// Scalar product.
	if( t1.x() * t2.x() + t1.y() * t2.y() >= VGlobal::parallelTolerance )
		return true;

	return false;
}

KoRect
VSegment::boundingBox() const
{
	// Initialize with p3.
	KoRect rect( m_node[2], m_node[2] );

	if( m_prev )
	{
		if( m_prev->m_node[2].x() < rect.left() )
			rect.setLeft( m_prev->m_node[2].x() );
		if( m_prev->m_node[2].x() > rect.right() )
			rect.setRight( m_prev->m_node[2].x() );
		if( m_prev->m_node[2].y() < rect.top() )
			rect.setTop( m_prev->m_node[2].y() );
		if( m_prev->m_node[2].y() > rect.bottom() )
			rect.setBottom( m_prev->m_node[2].y() );
	}

	if( m_type == curve )
	{
		if( m_node[0].x() < rect.left() )
			rect.setLeft( m_node[0].x() );
		if( m_node[0].x() > rect.right() )
			rect.setRight( m_node[0].x() );
		if( m_node[0].y() < rect.top() )
			rect.setTop( m_node[0].y() );
		if( m_node[0].y() > rect.bottom() )
			rect.setBottom( m_node[0].y() );

		if( m_node[1].x() < rect.left() )
			rect.setLeft( m_node[1].x() );
		if( m_node[1].x() > rect.right() )
			rect.setRight( m_node[1].x() );
		if( m_node[1].y() < rect.top() )
			rect.setTop( m_node[1].y() );
		if( m_node[1].y() > rect.bottom() )
			rect.setBottom( m_node[1].y() );
	}

	return rect;
}

VSegment*
VSegment::splitAt( double t )
{
	if( !m_prev || m_type == begin )
		return 0L;

	VSegment* segment = new VSegment();

	// Lines are easy: no need to modify the current segment.
	if( m_type == line )
	{
		segment->m_node[2] =
			m_prev->m_node[2] +
			( m_node[2] - m_prev->m_node[2] ) * t;

		segment->m_type = line;
		return segment;
	}

	// These references make our life a bit easier.
	KoPoint& p0 = m_prev->m_node[2];
	KoPoint& p1 = m_node[0];
	KoPoint& p2 = m_node[1];
	KoPoint& p3 = m_node[2];

	// Calculate the 2 new beziers.
	segment->m_node[0] = p0 + ( p1 - p0 ) * t;
	segment->m_node[1] = p1 + ( p2 - p1 ) * t;

	p2 = p2 + ( p3 - p2 ) * t;
	p1 = segment->m_node[1] + ( p2 - segment->m_node[1] ) * t;

	segment->m_node[1] =
		segment->m_node[0] + ( segment->m_node[1] - segment->m_node[0] ) * t;
	segment->m_node[2] =
		segment->m_node[1] + ( p1 - segment->m_node[1] ) * t;

	// Set the new segment type and state.
	segment->m_type = curve;
	segment->m_state = m_state;

	return segment;
}

void
VSegment::convertToCurve( double t )
{
	if(
		!m_prev ||
		m_type == begin )
	{
		return;
	}

	if( m_type == line )
	{
		m_node[0] = point( t );
		m_node[1] = point( 1.0 - t );
	}

	m_type = curve;
	m_ctrlPointFixing = none;
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

// TODO: Move this function into "userland"
uint
VSegment::nodeNear( const KoPoint& p, double isNearRange ) const
{
	if( m_node[0].isNear( p, isNearRange ) )
		return 1;

	if( m_node[1].isNear( p, isNearRange ) )
		return 2;

	if( m_node[2].isNear( p, isNearRange ) )
		return 3;

	return 0;
}

VSegment*
VSegment::revert() const
{
	if( !m_prev )
		return 0L;

	VSegment* segment = new VSegment();
	segment->m_state = m_state;
	segment->m_type = m_type;

	// Swap points.
	segment->m_node[0] = m_node[1];
	segment->m_node[1] = m_node[0];
	segment->m_node[2] = m_prev->m_node[2];

	// Swap node selection.
	segment->m_nodeSelected[0] = m_nodeSelected[1];
	segment->m_nodeSelected[1] = m_nodeSelected[0];
	segment->m_nodeSelected[2] = m_prev->m_nodeSelected[2];

	// Swap control point fixing.
	if( m_ctrlPointFixing == first )
		segment->m_ctrlPointFixing = second;
	else if( m_ctrlPointFixing == second )
		segment->m_ctrlPointFixing = first;

	return segment;
}

VSegment*
VSegment::prev() const
{
	VSegment* segment = m_prev;

	while( segment && segment->state() == deleted )
	{
		segment = segment->m_prev;
	}

	return segment;
}

VSegment*
VSegment::next() const
{
	VSegment* segment = m_next;

	while( segment && segment->state() == deleted )
	{
		segment = segment->m_next;
	}

	return segment;
}

void
VSegment::save( QDomElement& element ) const
{
	if( state() == deleted )
		return;

	QDomElement me;

	if( m_type == curve )
	{
		me = element.ownerDocument().createElement( "CURVE" );
		me.setAttribute( "x1", m_node[0].x() );
		me.setAttribute( "y1", m_node[0].y() );
		me.setAttribute( "x2", m_node[1].x() );
		me.setAttribute( "y2", m_node[1].y() );
		me.setAttribute( "x3", m_node[2].x() );
		me.setAttribute( "y3", m_node[2].y() );
	}
	else if( m_type == line )
	{
		me = element.ownerDocument().createElement( "LINE" );
		me.setAttribute( "x", m_node[2].x() );
		me.setAttribute( "y", m_node[2].y() );
	}
	else if( m_type == begin )
	{
		me = element.ownerDocument().createElement( "MOVE" );
		me.setAttribute( "x", m_node[2].x() );
		me.setAttribute( "y", m_node[2].y() );
	}

	if( m_ctrlPointFixing )
		me.setAttribute( "ctrlPointFixing", m_ctrlPointFixing );

	element.appendChild( me );
}

void
VSegment::load( const QDomElement& element )
{
	switch( element.attribute( "ctrlPointFixing", "0" ).toUShort() )
	{
		case 1:
			m_ctrlPointFixing = first; break;
		case 2:
			m_ctrlPointFixing = second; break;
		default:
			m_ctrlPointFixing = none;
	}

	if( element.tagName() == "CURVE" )
	{
		m_type = curve;
		m_node[0].setX( element.attribute( "x1" ).toDouble() );
		m_node[0].setY( element.attribute( "y1" ).toDouble() );
		m_node[1].setX( element.attribute( "x2" ).toDouble() );
		m_node[1].setY( element.attribute( "y2" ).toDouble() );
		m_node[2].setX( element.attribute( "x3" ).toDouble() );
		m_node[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "LINE" )
	{
		m_type = line;
		m_node[2].setX( element.attribute( "x" ).toDouble() );
		m_node[2].setY( element.attribute( "y" ).toDouble() );
	}
	else if( element.tagName() == "MOVE" )
	{
		m_type = begin;
		m_node[2].setX( element.attribute( "x" ).toDouble() );
		m_node[2].setY( element.attribute( "y" ).toDouble() );
	}
}

VSegment*
VSegment::clone() const
{
	return new VSegment( *this );
}

