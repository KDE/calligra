/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <math.h>

#include <qdom.h>
#include <q3valuelist.h>
#include <qmatrix.h>

#include "vpath.h"
#include "vsegment.h"
#include "vvisitor.h"

#include <kdebug.h>
#include <q3tl.h>


class VSubpathIteratorList
{
public:
	VSubpathIteratorList()
			: m_list( 0L ), m_iterator( 0L )
	{}

	~VSubpathIteratorList()
	{
		notifyClear( true );
		delete m_list;
	}

	void add( VSubpathIterator* itr )
	{
		if( !m_iterator )
			m_iterator = itr;
		else if( m_list )
			m_list->push_front( itr );
		else
		{
			m_list = new Q3ValueList<VSubpathIterator*>;
			m_list->push_front( itr );
		}
	}

	void remove( VSubpathIterator* itr )
	{
		if( m_iterator == itr )
			m_iterator = 0L;
		else if( m_list )
		{
			m_list->remove( itr );

			if( m_list->isEmpty() )
			{
				delete m_list;
				m_list = 0L;
			}
		}
	}

	void notifyClear( bool zeroList )
	{
		if( m_iterator )
		{
			if( zeroList )
				m_iterator->m_list = 0L;

			m_iterator->m_current = 0L;
		}

		if( m_list )
		{
			for(
				Q3ValueList<VSubpathIterator*>::Iterator itr = m_list->begin();
				itr != m_list->end();
				++itr )
			{
				if( zeroList )
					( *itr )->m_list = 0L;

				( *itr )->m_current = 0L;
			}
		}
	}

	void notifyRemove( VSegment* segment, VSegment* current )
	{
		if( m_iterator )
		{
			if( m_iterator->m_current == segment )
				m_iterator->m_current = current;
		}

		if( m_list )
		{
			for(
				Q3ValueList<VSubpathIterator*>::Iterator itr = m_list->begin();
				itr != m_list->end();
				++itr )
			{
				if( ( *itr )->m_current == segment )
					( *itr )->m_current = current;
			}
		}
	}

private:
	Q3ValueList<VSubpathIterator*>* m_list;
	VSubpathIterator* m_iterator;
};


VSubpath::VSubpath( VObject* parent )
		: VObject( parent )
{
	m_isClosed = false;

	m_first = m_last = m_current = 0L;
	m_number = 0;
	m_currentIndex = -1;
	m_iteratorList = 0L;

	// Add an initial segment.
	append( new VSegment( 1 ) );
}

VSubpath::VSubpath( const VSubpath& list )
		: VObject( list )
{
	m_isClosed = list.isClosed();

	m_first = m_last = m_current = 0L;
	m_number = 0;
	m_currentIndex = -1;
	m_iteratorList = 0L;

	VSegment* segment = list.m_first;

	while( segment )
	{
		append( segment->clone() );
		segment = segment->m_next;
	}
}

VSubpath::VSubpath( const VSegment& segment )
		: VObject( 0L )
{
	m_isClosed = false;

	m_first = m_last = m_current = 0L;
	m_number = 0;
	m_currentIndex = -1;
	m_iteratorList = 0L;

	// The segment is not a "begin" segment.
	if( segment.prev() )
	{
		// Add an initial segment.
		append( new VSegment( 1 ) );

		// Move the "begin" segment to the new segment's previous knot.
		moveTo( segment.prev()->knot() );
	}

	// Append a copy of the segment.
	append( segment.clone() );
}

VSubpath::~VSubpath()
{
	clear();
	delete m_iteratorList;
}

const QPointF&
VSubpath::currentPoint() const
{
	return getLast()->knot();
}

bool
VSubpath::moveTo( const QPointF& p )
{
	// Move "begin" segment if path is still empty.
	if( isEmpty() )
	{
		getLast()->setKnot( p );
		return true;
	}

	return false;
}

bool
VSubpath::lineTo( const QPointF& p )
{
	if( isClosed() )
		return false;

	VSegment* s = new VSegment( 1 );

	s->setDegree( 1 );
	s->setKnot( p );

	append( s );

	return true;
}

bool
VSubpath::curveTo(
	const QPointF& p1, const QPointF& p2, const QPointF& p3 )
{
	if( isClosed() )
		return false;

	VSegment* s = new VSegment();

	s->setDegree( 3 );
	s->setPoint( 0, p1 );
	s->setPoint( 1, p2 );
	s->setPoint( 2, p3 );

	append( s );


	return true;
}

bool
VSubpath::curve1To( const QPointF& p2, const QPointF& p3 )
{
	if( isClosed() )
		return false;

	VSegment* s = new VSegment();

	s->setDegree( 3 );
	s->setPoint( 0, currentPoint() );
	s->setPoint( 1, p2 );
	s->setPoint( 2, p3 );

	append( s );


	return true;
}

bool
VSubpath::curve2To( const QPointF& p1, const QPointF& p3 )
{
	if( isClosed() )
		return false;

	VSegment* s = new VSegment();

	s->setDegree( 3 );
	s->setPoint( 0, p1 );
	s->setPoint( 1, p3 );
	s->setPoint( 2, p3 );

	append( s );


	return true;
}

bool
VSubpath::arcTo(
	const QPointF& p1, const QPointF& p2, const double r )
{
	/* This routine is inspired by code in GNU ghostscript.
	 *
	 *           |- P1B3 -|
	 *
	 *          |- - - T12- - -|
	 *
	 *  -   - P1 x....__--o.....x P2
	 *  |   |    :  _/    B3
	 * P1B0      : /
	 *      |    :/
	 *  |        |
	 *  -  T10   o B0
	 *           |
	 *      |    |
	 *           |
	 *      |    |
	 *      -    x P0
	 */

	if( isClosed() || r < 0.0 )
		return false;


	// We need to calculate the tangent points. Therefore calculate tangents
	// T10=P1P0 and T12=P1P2 first.
	double dx0 = currentPoint().x() - p1.x();
	double dy0 = currentPoint().y() - p1.y();
	double dx2 = p2.x() - p1.x();
	double dy2 = p2.y() - p1.y();

	// Calculate distance squares.
	double dsqT10 = dx0 * dx0 + dy0 * dy0;
	double dsqT12 = dx2 * dx2 + dy2 * dy2;

	// We now calculate tan(a/2) where a is the angle between T10 and T12.
	// We benefit from the facts T10*T12 = |T10|*|T12|*cos(a),
	// |T10xT12| = |T10|*|T12|*sin(a) (cross product) and tan(a/2) = sin(a)/[1-cos(a)].
	double num = dy0 * dx2 - dy2 * dx0;

	double denom = sqrt( dsqT10 * dsqT12 ) - ( dx0 * dx2 + dy0 * dy2 );

	// The points are colinear.
	if( 1.0 + denom == 1.0 )
	{
		// Just add a line.
		lineTo( p1 );
	}
	else
	{
		// |P1B0| = |P1B3| = r * tan(a/2).
		double dP1B0 = fabs( r * num / denom );

		// B0 = P1 + |P1B0| * T10/|T10|.
		QPointF b0 = p1 + QPointF( dx0, dy0 ) * ( dP1B0 / sqrt( dsqT10 ) );

		// If B0 deviates from current point P0, add a line to it.
		if( (currentPoint().x() >= b0.x() - VGlobal::isNearRange && currentPoint().x() <= b0.x() + VGlobal::isNearRange && currentPoint().y() >= b0.y() - VGlobal::isNearRange && currentPoint().y() <= b0.y() + VGlobal::isNearRange) )
			lineTo( b0 );

		// B3 = P1 + |P1B3| * T12/|T12|.
		QPointF b3 = p1 + QPointF( dx2, dy2 ) * ( dP1B0 / sqrt( dsqT12 ) );


		// The two bezier-control points are located on the tangents at a fraction
		// of the distance[ tangent points <-> tangent intersection ].
		const QPointF d = p1 - b0;

		double distsq = d.x() * d.x() + d.y() * d.y();

		double rsq = r * r;

		double fract;

		// r is very small.
		if( distsq >= rsq * VGlobal::veryBigNumber )
		{
			// Assume dist = r = 0.
			fract = 0.0;
		}
		else
		{
			fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ) );
		}

		QPointF b1 = b0 + ( p1 - b0 ) * fract;
		QPointF b2 = b3 + ( p1 - b3 ) * fract;

		// Finally add the bezier-segment.
		curveTo( b1, b2, b3 );
	}

	return true;
}

void
VSubpath::close()
{
	// In the case the list is 100% empty (which should actually never happen),
	// append a "begin" first, to avoid a crash.
	if( count() == 0 )
		append( new VSegment( 1 ) );

	// Move last segment if we are already closed.
	if( isClosed() )
	{
		getLast()->setKnot( getFirst()->knot() );
	}
	// Append a line, if necessary.
	else
	{
		if((getFirst()->knot().x() >= getLast()->knot().x() - VGlobal::isNearRange && getFirst()->knot().x() <= getLast()->knot().x() + VGlobal::isNearRange && getFirst()->knot().y() >= getLast()->knot().y() - VGlobal::isNearRange && getFirst()->knot().y() <= getLast()->knot().y() + VGlobal::isNearRange))
		{
			// Move last knot.
			getLast()->setKnot( getFirst()->knot() );
		}
		else
		{
			// Add a line.
			lineTo( getFirst()->knot() );
		}

		m_isClosed = true;
	}
}

bool
VSubpath::pointIsInside( const QPointF& p ) const
{
	// If the point is not inside the boundingbox, it cannot be inside the path either.
	if( !boundingBox().contains( p ) )
		return false;

	// First check if the point is inside the knot polygon (beziers are treated
	// as lines).

	/* This algorithm is taken from "Fast Winding Number Inclusion of a Point
	 * in a Polygon" by Dan Sunday, geometryalgorithms.com.
	 */

	/*
	int windingNumber = 0;

	// Ommit first segment.
	VSegment* segment = getFirst()->next();

	while( segment )
	{
		if( segment->prev()->knot().y() <= p.y() )
		{
			// Upward crossing.
			if( segment->knot().y() > p.y() )
			{
				// Point is left.
				if( segment->pointIsLeft( p ) > 0 )
				{
					// Valid up intersection.
					++windingNumber;
				}
			}
		}
		else
		{
			// Downward crossing.
			if( segment->knot().y() <= p.y() )
			{
				// Point is right.
				if( segment->pointIsLeft( p ) < 0 )
				{
					// Valid down intersection.
					--windingNumber;
				}
			}
		}

		segment = segment->next();
	}

	if( static_cast<bool>( windingNumber ) )
		return true;
	*/
	
	// Then check if the point is located in between the knot polygon
	// and the bezier curves.

	/* We rotate each segment in order to make their chord (the line between
	 * the previous knot and the knot ) parallel to the x-axis. Then we
	 * calculate y(xp) on the segment for the rotated input point (xp,yp)
	 * and compare y(xp) with yp.
	 */
// TODO
	
	// cache the closed evaluation
	bool closed = isClosed() || getLast()->knot() == getFirst()->knot();

	Q3ValueList<double> rparams;

	VSegment* segment = getFirst()->next();

	// move all segements so that p is the origin 
	// and compute their intersections with the x-axis
	while( segment )
	{
		VSubpath tmpCurve( 0L );
		tmpCurve.append( new VSegment( segment->degree() ) );
	
		for( int i = 0; i <= segment->degree(); ++i )
			tmpCurve.current()->setP(i, segment->p(i)-p );
		
		tmpCurve.current()->rootParams( rparams );

		segment = segment->next();
	}
	
	// if the path is not closed, compute the intersection of
	// the line through the first and last knot and the x-axis too
	if( ! closed )
	{
		QPointF prevKnot = getLast()->knot() - p;
		QPointF nextKnot = getFirst()->knot() - p;

		double dx = nextKnot.x() - prevKnot.x();
		double dy = nextKnot.y() - prevKnot.y();
		if( dx == 0.0 )
		{
			rparams.append( nextKnot.x() );
		}
		else if( dy != 0.0 )
		{
			if( ( prevKnot.y() < 0.0 && nextKnot.y() > 0.0 ) || ( prevKnot.y() > 0.0 && nextKnot.y() < 0.0 ) )
			{
				double n = prevKnot.y() - dy / dx * prevKnot.x();
				rparams.append( -n * dx / dy );
			}
		}
	}
	
	kDebug(38000) << "intersection count: " << rparams.count() << endl;

	// sort all intersections
	qHeapSort( rparams );

	Q3ValueList<double>::iterator itr, etr = rparams.end();
	
	for( itr = rparams.begin(); itr != etr; ++itr )
		kDebug(38000) << "intersection: " << *itr << endl;

	if( closed )
	{
		// pair the intersections and check if the origin is within a pair
		for( itr = rparams.begin(); itr != etr; ++itr )
		{
			if( *itr > 0.0 ) 
				return false;
	
			if( ++itr == etr )
				return false;
			
			if( *itr > 0.0 )
				return true;
		}
	}
	else
	{
		// only check if point is between first and last intersection if we have an open path
		if ( rparams.front() < 0.0 && rparams.back() > 0.0 )
			return true;
	}

	return false;
}

bool
VSubpath::intersects( const VSegment& s ) const
{
	// Check if path is empty and if boundingboxes intersect.
	if(
		isEmpty() ||
		!boundingBox().intersects( s.boundingBox() ) )
	{
		return false;
	}


	// Ommit first segment.
	VSegment* segment = getFirst()->next();

	while( segment )
	{
		if( segment->intersects( s ) )
		{
			return true;
		}

		segment = segment->next();
	}

	return false;
}

bool
VSubpath::counterClockwise() const
{
	/* This algorithm is taken from the FAQ of comp.graphics.algorithms:
	 * "Find the lowest vertex (or, if there is more than one vertex with the
	 * same lowest coordinate, the rightmost of those vertices) and then take
	 * the cross product of the edges fore and aft of it."
	 */

	// A non closed path does not have a winding.
	if( !isClosed() )
	{
		return false;
	}


	VSegment* segment = getFirst();

	// We save the segment not the knot itself. Initialize it with the
	// first segment:
	const VSegment* bottomRight = getFirst();

	while( segment )
	{
		if( segment->knot().y() < bottomRight->knot().y() )
			bottomRight = segment;
		else if( segment->knot().y() - bottomRight->knot().y()
				  < VGlobal::isNearRange )
		{
			if( segment->knot().x() > bottomRight->knot().x() )
				bottomRight = segment;
		}

		segment = segment->next();
	}


	// Catch boundary case (bottomRight is first or last segment):
	const VSegment* current;
	const VSegment* next;

	if( bottomRight == getFirst() )
		current = getLast();
	else
		current = bottomRight;

	if( bottomRight == getLast() )
		next = getFirst()->next();
	else
		next = bottomRight->next();

	// Check "z-component" of cross product:
	return
		( next->knot().x() - next->prev()->knot().x() ) *
		( current->knot().y() - current->prev()->knot().y() )
		-
		( next->knot().y() - next->prev()->knot().y() ) *
		( current->knot().x() - current->prev()->knot().x() ) < 0.0;
}

void
VSubpath::revert()
{
	// Catch case where the list is "empty".
	if( isEmpty() )
		return;


	VSubpath list( parent() );
	list.moveTo( getLast()->knot() );

	VSegment* segment = getLast();

	while( segment->prev() )
	{
		list.append( segment->revert() );
		segment = segment->prev();
	}

	list.m_isClosed = isClosed();

	*this = list;
}

const QRectF&
VSubpath::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// Reset the boundingbox.
		m_boundingBox = QRectF();

		VSegment* segment = m_first;

		while( segment )
		{
			if( segment->state() != VSegment::deleted )
				m_boundingBox |= segment->boundingBox();

			segment = segment->m_next;
		}

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

VSubpath*
VSubpath::clone() const
{
	return new VSubpath( *this );
}

void
VSubpath::saveSvgPath( QString &d ) const
{
	// Save segments.
	VSegment* segment = getFirst();

	while( segment )
	{
		if( segment->state() == VSegment::normal )
		{
			if( segment->degree() <= 2 )
			{
				// Line.
				if( segment->prev() )
				{
					d += QString( "L%1 %2" ).
							arg( segment->knot().x() ).arg( segment->knot().y() );
				}
				// Moveto.
				else
				{
					d += QString( "M%1 %2" ).
							arg( segment->knot().x() ).arg( segment->knot().y() );
				}
			}
			// Bezier ( degree >= 3 ).
			else
			{
				// We currently treat all beziers as cubic beziers.
				d += QString( "C%1 %2 %3 %4 %5 %6" ).
							arg( segment->point( segment->degree() - 3 ).x() ).
							arg( segment->point( segment->degree() - 3 ).y() ).
							arg( segment->point( segment->degree() - 2 ).x() ).
							arg( segment->point( segment->degree() - 2 ).y() ).
							arg( segment->knot().x() ).
							arg( segment->knot().y() );
			}
		}

		segment = segment->m_next;
	}

	if( isClosed() )
		d += "Z";
}

// TODO: remove this backward compatibility function after koffice 1.3.x
void
VSubpath::load( const QDomElement& element )
{
	// We might have a "begin" segment.
	clear();

	QDomNodeList list = element.childNodes();

	for( int i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement segment = list.item( i ).toElement();

			VSegment* s = new VSegment();
			s->load( segment );
			append( s );
		}
	}

	if( element.attribute( "isClosed" ) == 0 ? false : true )
		close();
}

void
VSubpath::accept( VVisitor& visitor )
{
	visitor.visitVSubpath( *this );
}


VSubpath&
VSubpath::operator=( const VSubpath& list )
{
	if( this == &list )
		return *this;

	m_isClosed = list.isClosed();

	clear();

	VSegment* segment = list.m_first;

	while( segment )
	{
		append( segment->clone() );
		segment = segment->m_next;
	}

	m_current = m_first;
	m_currentIndex = 0;

	return *this;
}

bool
VSubpath::insert( const VSegment* segment )
{
	if( m_currentIndex == -1 )
		return false;

	VSegment* s = const_cast<VSegment*>( segment );

	VSegment* prev = m_current->m_prev;

	m_current->m_prev = s;
	prev->m_next = s;
	s->m_prev = prev;
	s->m_next = m_current;

	m_current = s;
	++m_number;

	invalidateBoundingBox();

	return true;
}

bool
VSubpath::insert( uint index, const VSegment* segment )
{
	VSegment* s = const_cast<VSegment*>( segment );

	if( index == 0 )
	{
		prepend( s );
		return true;
	}
	else if( index == m_number )
	{
		append( s );
		return true;
	}

	VSegment* next = locate( index );

	if( !next )
		return false;

	VSegment* prev = next->m_prev;

	next->m_prev = s;
	prev->m_next = s;
	s->m_prev = prev;
	s->m_next = next;

	m_current = s;
	++m_number;

	invalidateBoundingBox();

	return true;
}

void
VSubpath::prepend( const VSegment* segment )
{
	VSegment* s = const_cast<VSegment*>( segment );

	s->m_prev = 0L;

	if( ( s->m_next = m_first ) )
		m_first->m_prev = s;
	else
		m_last = s;

	m_first = m_current = s;

	++m_number;
	m_currentIndex = 0;

	invalidateBoundingBox();
}

void
VSubpath::append( const VSegment* segment )
{
	VSegment* s = const_cast<VSegment*>( segment );

	s->m_next = 0L;

	if( ( s->m_prev = m_last ) )
		m_last->m_next = s;
	else
		m_first = s;

	m_last = m_current = s;

	m_currentIndex = m_number;
	++m_number;

	invalidateBoundingBox();
}

void
VSubpath::clear()
{
	VSegment* segment = m_first;

	m_first = m_last = m_current = 0L;
	m_number = 0;
	m_currentIndex = -1;

	if( m_iteratorList )
		m_iteratorList->notifyClear( false );

	VSegment* prev;

	while( segment )
	{
		prev = segment;
		segment = segment->m_next;
		delete prev;
	}

	m_isClosed = false;

	invalidateBoundingBox();
}

VSegment*
VSubpath::first()
{
	if( m_first )
	{
		m_currentIndex = 0;
		return m_current = m_first;
	}

	return 0L;
}

VSegment*
VSubpath::last()
{
	if( m_last )
	{
		m_currentIndex = m_number - 1;
		return m_current = m_last;
	}

	return 0L;
}

VSegment*
VSubpath::prev()
{
	if( m_current )
	{
		if( m_current->m_prev )
		{
			--m_currentIndex;
			return m_current = m_current->m_prev;
		}

		m_currentIndex = -1;
		m_current = 0L;
	}

	return 0L;
}

VSegment*
VSubpath::next()
{
	if( m_current )
	{
		if( m_current->m_next )
		{
			++m_currentIndex;
			return m_current = m_current->m_next;
		}

		m_currentIndex = -1;
		m_current = 0L;
	}

	return 0L;
}

VSegment*
VSubpath::locate( uint index )
{
	if( index == static_cast<uint>( m_currentIndex ) )
		return m_current;

	if( !m_current && m_first )
	{
		m_current = m_first;
		m_currentIndex = 0;
	}

	VSegment* segment;
	int distance = index - m_currentIndex;
	bool forward;

	if( index >= m_number )
		return 0L;

	if( distance < 0 )
		distance = -distance;

	if(
		static_cast<uint>( distance ) < index &&
		static_cast<uint>( distance ) < m_number - index )
	{
		segment = m_current;
		forward = index > static_cast<uint>( m_currentIndex );
	}
	else if( index < m_number - index )
	{
		segment = m_first;
		distance = index;
		forward = true;
	}
	else
	{
		segment = m_last;
		distance = m_number - index - 1;
		if( distance < 0 )
			distance = 0;
		forward = false;
	}

	if( forward )
	{
		while( distance-- )
			segment = segment->m_next;
	}
	else
	{
		while( distance-- )
			segment = segment->m_prev;
	}

	m_currentIndex = index;
	return m_current = segment;
}


VSubpathIterator::VSubpathIterator( const VSubpath& list )
{
	m_list = const_cast<VSubpath*>( &list );
	m_current = m_list->m_first;

	if( !m_list->m_iteratorList )
		m_list->m_iteratorList = new VSubpathIteratorList();

	m_list->m_iteratorList->add( this );
}

VSubpathIterator::VSubpathIterator( const VSubpathIterator& itr )
{
	m_list = itr.m_list;
	m_current = itr.m_current;

	if( m_list )
		m_list->m_iteratorList->add( this );
}

VSubpathIterator::~VSubpathIterator()
{
	if( m_list )
		m_list->m_iteratorList->remove( this );
}

VSubpathIterator&
VSubpathIterator::operator=( const VSubpathIterator& itr )
{
	if( m_list )
		m_list->m_iteratorList->remove( this );

	m_list = itr.m_list;
	m_current = itr.m_current;

	if( m_list )
		m_list->m_iteratorList->add( this );

	return *this;
}

VSegment*
VSubpathIterator::current() const
{
	// If m_current points to a deleted segment, find the next not
	// deleted segment.
	if(
		m_current &&
		m_current->state() == VSegment::deleted )
	{
		return m_current->next();
	}

	return m_current;
}

VSegment*
VSubpathIterator::operator()()
{
	if( VSegment* const old = current() )
	{
		m_current = current()->next();
		return old;
	}

	return 0L;
}

VSegment*
VSubpathIterator::operator++()
{
	if( current() )
		return m_current = current()->next();

	return 0L;
}

VSegment*
VSubpathIterator::operator+=( uint i )
{
	while( current() && i-- )
		m_current = current()->next();

	return current();
}

VSegment*
VSubpathIterator::operator--()
{
	if( current() )
		return m_current = current()->prev();

	return 0L;
}

VSegment*
VSubpathIterator::operator-=( uint i )
{
	while( current() && i-- )
		m_current = current()->prev();

	return current();
}

