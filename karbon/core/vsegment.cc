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
	const KoPoint ab = b - a;
	const double norm = sqrt( ab * ab );

	// If norm is very small, simply use distance AP.
	if( norm < VGlobal::verySmallNumber )
		return
			sqrt(
				( p.x() - a.x() ) * ( p.x() - a.x() ) +
				( p.y() - a.y() ) * ( p.y() - a.y() ) );

	// Normalize.
	return QABS( det ) / norm;
}


VSegment::VSegment( unsigned degree )
{
	m_nodes = new VNodeData[ degree ];
	m_type = begin;
	m_state = normal;

	m_prev = 0L;
	m_next = 0L;

	m_nodeSelected[0] = false;
	m_nodeSelected[1] = false;
	m_nodeSelected[2] = false;

	m_nodeEdited[0] = false;
	m_nodeEdited[1] = false;
	m_nodeEdited[2] = false;


	m_ctrlPointFixing = none;
}

VSegment::VSegment( const VSegment& segment )
{
	m_nodes = new VNodeData[ segment.m_degree ];
	m_type = segment.m_type;
	m_state = segment.m_state;

	// Copying the pointers m_prev/m_next has some advantages (see VSegment::length()).
	// Inserting a segment into a path overwrites these anyway.
	m_prev = segment.m_prev;
	m_next = segment.m_next;

	// Copy points.
	for( unsigned i = 0; i < degree(); ++i )
	{
		setPoint( i, segment.point( i ) );
	}

	m_nodeSelected[0] = segment.m_nodeSelected[0];
	m_nodeSelected[1] = segment.m_nodeSelected[1];
	m_nodeSelected[2] = segment.m_nodeSelected[2];

	m_nodeEdited[0] = segment.m_nodeEdited[0];
	m_nodeEdited[1] = segment.m_nodeEdited[1];
	m_nodeEdited[2] = segment.m_nodeEdited[2];

	m_ctrlPointFixing = segment.m_ctrlPointFixing;
}

VSegment::~VSegment()
{
	delete[]( m_nodes );
}

void
VSegment::setDegree( unsigned degree )
{
	// Do nothing if old and new degrees are identical.
	if( m_degree == degree )
		return;


	// Delete old node data.
	delete[]( m_nodes );

	// Allocate new node data.
	m_nodes = new VNodeData[ degree ];
}

void
VSegment::draw( VPainter* painter ) const
{
	if( state() == deleted )
		return;


	if( type() == curve )
	{
		painter->curveTo( point( 0 ), point( 1 ), point( 3 ) );
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
	{
		bool flat;

		for( unsigned i = 0; i < degree() - 1; ++i )
		{
			flat =
				height( m_prev->knot(), point( i ), knot() ) / chordLength()
					< flatness;

			if( !flat )
				break;
		}

		return flat;
	}

	return false;
}

KoPoint
VSegment::at( double t ) const
{
	KoPoint p;

	pointDerivatives( t, &p );

	return p;
}

void
VSegment::pointDerivatives( double t, KoPoint* p,
	KoPoint* d1, KoPoint* d2 ) const
{
	if(
		!m_prev ||
		m_type == begin )
	{
		return;
	}


	// Lines.
	if( m_type == line )
	{
		const KoPoint diff = knot() - m_prev->knot();

		if( p )
			*p = m_prev->knot() + diff * t;
		if( d1 )
			*d1 = diff;
		if( d2 )
			*d2 = KoPoint( 0.0, 0.0 );

		return;
	}


	// Beziers.

	// Copy points.
	KoPoint* q = new KoPoint[ degree() + 1 ];

	q[ 0 ] = m_prev->knot();

	for( unsigned i = 0; i < degree(); ++i )
	{
		q[ i + 1 ] = point( i );
	}


	// The De Casteljau algorithm.
	for( uint j = 1; j <= degree(); ++j )
	{
		for( uint i = 0; i <= degree() - j; ++i )
		{
			q[i] = ( 1.0 - t ) * q[i] + t * q[i+1];
		}

		// Save first derivative now we have it.
		if( j == 1 )
		{
			if( d2 )
				*d2 = 6 * ( q[2] - 2 * q[1] + q[0] );
		}
		// Save second derivative now we have it.
		else if( j == 2 )
		{
			if( d1 )
				*d1 = 3 * ( q[1] - q[0] );
		}
	}

	// Save point.
	if( p )
		*p = q[0];


	delete[]( q );


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
	if(
		!m_prev ||
		m_type == begin ||
		t == 0.0 )
	{
		return 0.0;
	}

	// Length of a line.
	if( m_type == line )
	{
		return
			t * chordLength();
	}
	// Length of a bezier.
	else if( m_type == curve )
	{
		/* The idea for this algortihm is by Jens Gravesen <gravesen@mat.dth.dk>.
		 * We calculate the chord length "chord"=|P0P3| and the length of the control point
		 * polygon "poly"=|P0P1|+|P1P2|+|P2P3|. The approximation for the bezier length is
		 * 0.5 * poly + 0.5 * chord. "poly - chord" is a measure for the error.
		 * We subdivide each segment until the error is smaller than a given tolerance
		 * and add up the subresults.
		 */

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
	if(
		!m_prev ||
		m_type == begin )
	{
		return 0.0;
	}

	return
		sqrt(
				( knot().x() - m_prev->knot().x() ) *
				( knot().x() - m_prev->knot().x() ) +
				( knot().y() - m_prev->knot().y() ) *
				( knot().y() - m_prev->knot().y() ) );
}

double
VSegment::polyLength() const
{
	if(
		!m_prev ||
		m_type == begin )
	{
		return 0.0;
	}


	KoPoint d = point( 0 ) - m_prev->knot();
	double length = sqrt( d * d );

	for( unsigned i = 1; i < degree(); ++i )
	{
		d = point( i ) - point( i - 1 );
		length += sqrt( d * d );
	}


	return length;
}

double
VSegment::param( double len ) const
{
	if(
		len == 0.0 ||		// We divide by len below.
		!m_prev ||
		m_type == begin )
	{
		return 0.0;
	}

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

double
VSegment::nearestPointParam( const KoPoint& /*p*/ ) const
{
	if(
		!m_prev ||
		m_type == begin )
	{
		return 1.0;
	}

	/* This function solves the "nearest point on curve" problem. That means, it
	 * calculates the point q (to be precise: it's paramter t) on this segment, which
	 * is located nearest to the input point p.
	 * The basic idea is described e.g. in "The NURBS Book" by Les Piegl, Wayne Tiller
	 * (Springer 1997) or "Solving the Nearest Point-on-Curve Problem" and "A Bezier
	 * Curve-Based Root-Finder" by Philip J. Schneider (from "Graphics Gems",
	 * Academic Press 1990).
	 *
	 * For the nearest point q = C(t) on this segment, the first derivative is
	 * orthogonal to the distance vector "C(t) - p". In other words we are looking for
	 * solutions of f(t) = C'(t) * ( C(t) - p ) = 0.
	 * ( C(t) - p ) is a cubic polynom, C'(t) is quadratic => f(t) is a 5th degree
	 * polynom and thus has up to 5 distinct solutions.
	 * To solve it, we apply the newton iteration: a parameter approximation t_i leads
	 * to the next approximation t_{i+1}
	 *
	 *                  f(t_i)
	 * t_{i+1} = t_i - -------
	 *                 f'(t_i)
	 *
	 * Convergence criteria are then
	 *
	 * 1) Point coincidence: | C(t_i) - p | <= tolerance1
	 *
	 * 2) Zero cosine: | C'(t_i) * ( C(t_i) - p ) |
	 *                 ---------------------------- <= tolerance2
	 *                 | C'(t_i) | * | C(t_i) - p |
	 */

// TODO
	return 0.0;
}

bool
VSegment::isSmooth( const VSegment& next ) const
{
	// Return false if this segment is a "begin".
	if( type() == begin )
		return false;


	// Calculate tangents.
	KoPoint t1;
	KoPoint t2;

	pointTangentNormal( 1.0, 0L, &t1 );
	next.pointTangentNormal( 0.0, 0L, &t2 );


	// Scalar product.
	if( t1 * t2 >= VGlobal::parallelTolerance )
		return true;

	return false;
}

KoRect
VSegment::boundingBox() const
{
	// Initialize with knot.
	KoRect rect( knot(), knot() );


	// Add p0, if it exists.
	if( m_prev )
	{
		if( m_prev->knot().x() < rect.left() )
			rect.setLeft( m_prev->knot().x() );

		if( m_prev->knot().x() > rect.right() )
			rect.setRight( m_prev->knot().x() );

		if( m_prev->knot().y() < rect.top() )
			rect.setTop( m_prev->knot().y() );

		if( m_prev->knot().y() > rect.bottom() )
			rect.setBottom( m_prev->knot().y() );
	}


	for( unsigned i = 0; i < degree() - 1; ++i )
	{
		if( point( i ).x() < rect.left() )
			rect.setLeft( point( i ).x() );

		if( point( i ).x() > rect.right() )
			rect.setRight( point( i ).x() );

		if( point( i ).y() < rect.top() )
			rect.setTop( point( i ).y() );

		if( point( i ).y() > rect.bottom() )
			rect.setBottom( point( i ).y() );
	}


	return rect;
}

VSegment*
VSegment::splitAt( double t )
{
	if(
		!m_prev ||
		m_type == begin )
	{
		return 0L;
	}


	VSegment* segment = new VSegment( m_degree );

	// Lines are easy: no need to modify the current segment.
	if( m_type == line )
	{
		segment->setKnot(
			m_prev->knot() +
			( knot() - m_prev->knot() ) * t );

		segment->m_type = line;
		segment->m_state = m_state;

		return segment;
	}

/*
// TODO
	// These references make the folowing code nicer to look at.
	KoPoint& p0 = m_prev->knot();
	KoPoint& p1 = m_node[0];
	KoPoint& p2 = m_node[1];
	KoPoint& p3 = knot();

	// Calculate the 2 new beziers.
	segment->m_node[0] = p0 + ( p1 - p0 ) * t;
	segment->m_node[1] = p1 + ( p2 - p1 ) * t;

	p2 = p2 + ( p3 - p2 ) * t;
	p1 = segment->m_node[1] + ( p2 - segment->m_node[1] ) * t;

	segment->m_node[1] =
		segment->m_node[0] + ( segment->m_node[1] - segment->m_node[0] ) * t;
	segment->knot() =
		segment->m_node[1] + ( p1 - segment->m_node[1] ) * t;

	// Set the new segment type and state.
	segment->m_type = curve;
	segment->m_state = m_state;
*/

	return segment;
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
	unsigned index = 0;

	for( unsigned i = 0; i < degree(); ++i )
	{
		if( point( 0 ).isNear( p, isNearRange ) )
		{
			index = i + 1;
			break;
		}
	}

	return index;
}

VSegment*
VSegment::revert() const
{
	if(
		!m_prev ||
		m_type == begin )
	{
		return 0L;
	}

	VSegment* segment = new VSegment( m_degree );

	segment->m_type = m_type;
	segment->m_state = m_state;


	// Swap points.
	for( unsigned i = 0; i < degree() - 1; ++i )
	{
		segment->setPoint( i, point( degree() - 2 - i ) );
	}

	segment->setKnot( m_prev->knot() );


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
VSegment::transform( const QWMatrix& m )
{
	for( unsigned i = 0; i < degree(); ++i )
	{
		setPoint( i, point( i ).transform( m ) );
	}
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
		me.setAttribute( "x1", point( 0 ).x() );
		me.setAttribute( "y1", point( 0 ).y() );
		me.setAttribute( "x2", point( 1 ).x() );
		me.setAttribute( "y2", point( 1 ).y() );
		me.setAttribute( "x3", knot().x() );
		me.setAttribute( "y3", knot().y() );
	}
	else if( m_type == line )
	{
		me = element.ownerDocument().createElement( "LINE" );
		me.setAttribute( "x", knot().x() );
		me.setAttribute( "y", knot().y() );
	}
	else if( m_type == begin )
	{
		me = element.ownerDocument().createElement( "MOVE" );
		me.setAttribute( "x", knot().x() );
		me.setAttribute( "y", knot().y() );
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
			m_ctrlPointFixing = first;
			break;
		case 2:
			m_ctrlPointFixing = second;
			break;
		default:
			m_ctrlPointFixing = none;
	}

	if( element.tagName() == "CURVE" )
	{
		m_type = curve;

		setDegree( 3 );

		setPoint(
			0,
			KoPoint(
				element.attribute( "x1" ).toDouble(),
				element.attribute( "y1" ).toDouble() ) );

		setPoint(
			1,
			KoPoint(
				element.attribute( "x2" ).toDouble(),
				element.attribute( "y2" ).toDouble() ) );

		setKnot(
			KoPoint(
				element.attribute( "x3" ).toDouble(),
				element.attribute( "y3" ).toDouble() ) );
	}
	else if( element.tagName() == "LINE" )
	{
		m_type = line;

		setDegree( 1 );

		setKnot(
			KoPoint(
				element.attribute( "x" ).toDouble(),
				element.attribute( "y" ).toDouble() ) );
	}
	else if( element.tagName() == "MOVE" )
	{
		m_type = begin;

		setDegree( 1 );

		setKnot(
			KoPoint(
				element.attribute( "x" ).toDouble(),
				element.attribute( "y" ).toDouble() ) );
	}
}

VSegment*
VSegment::clone() const
{
	return new VSegment( *this );
}

