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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include <math.h>

#include <qdom.h>
#include <qvaluelist.h>
#include <qwmatrix.h>

#include "vpath.h"
#include "vsegment.h"
#include "vvisitor.h"

#include <kdebug.h>


class VPathIteratorList
{
public:
	VPathIteratorList()
		: m_list( 0L ), m_iterator( 0L ) {}

	~VPathIteratorList()
	{
		notifyClear( true );
		delete m_list;
	}

	void add( VPathIterator* itr )
	{
		if( !m_iterator )
			m_iterator = itr;
		else if( m_list )
			m_list->push_front( itr );
		else
		{
			m_list = new QValueList<VPathIterator*>;
			m_list->push_front( itr );
		}
	}

	void remove( VPathIterator* itr ) {
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
				QValueList<VPathIterator*>::Iterator itr = m_list->begin();
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
				QValueList<VPathIterator*>::Iterator itr = m_list->begin();
				itr != m_list->end();
				++itr )
			{
			if( ( *itr )->m_current == segment )
				( *itr )->m_current = current;
			}
		}
	}

private:
	QValueList<VPathIterator*>* m_list;
	VPathIterator* m_iterator;
};


VPath::VPath( VObject* parent )
	: VObject( parent )
{
	m_isClosed = false;

	m_first = m_last = m_current = 0L;
	m_number = 0;
	m_currentIndex = -1;
	m_iteratorList = 0L;

	// add an initial ("begin") segment:
	append( new VSegment() );
}

VPath::VPath( const VPath& list )
	: VObject( list )
{
	m_isClosed = list.m_isClosed;

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

VPath::VPath( const VSegment& segment )
	: VObject( 0L )
{
	m_isClosed = false;

	m_first = m_last = m_current = 0L;
	m_number = 0;
	m_currentIndex = -1;
	m_iteratorList = 0L;

	// add an initial ("begin") segment:
	append( new VSegment() );

	if( segment.prev() )
		moveTo( segment.prev()->knot() );

	append( segment.clone() );
}

VPath::~VPath()
{
	clear();
	delete m_iteratorList;
}

const KoPoint&
VPath::currentPoint() const
{
	return getLast()->knot();
}

bool
VPath::moveTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	// move "begin" when path is still empty:
	if( getLast()->type() == VSegment::begin )
	{
		getLast()->setKnot( p );
		return true;
	}

	return false;
}

bool
VPath::lineTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment();
	s->setType( VSegment::line );
	s->setKnot( p );
	append( s );

	return true;
}

bool
VPath::curveTo(
	const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment( VSegment::curve );
	s->setCtrlPoint1( p1 );
	s->setCtrlPoint2( p2 );
	s->setKnot( p3 );
	append( s );

	return true;
}

bool
VPath::curve1To( const KoPoint& p2, const KoPoint& p3 )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment( VSegment::curve );
	s->setCtrlPointFixing( VSegment::first );
	s->setCtrlPoint2( p2 );
	s->setKnot( p3 );
	append( s );

	return true;
}

bool
VPath::curve2To( const KoPoint& p1, const KoPoint& p3 )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment( VSegment::curve );
	s->setCtrlPointFixing( VSegment::second );
	s->setCtrlPoint1( p1 );
	s->setKnot( p3 );
	append( s );

	return true;
}

bool
VPath::arcTo(
	const KoPoint& p1, const KoPoint& p2, const double r )
{
	// This routine is inspired by code in GNU ghostscript.

	//           |- P1B3 -|
	//
	//           |- - - T12- - -|
	//
	//  -   - P1 x....__--o.....x P2
	//  |   |    :  _/    B3
	// P1B0      : /
	//      |    :/
	//  |        |
	//  -  T10   o B0
	//           |
	//      |    |
	//           |
	//      |    |
	//      -    x P0

	if( isClosed() || r < 0.0 ) return false;

	// We need to calculate the tangent points. Therefore calculate tangents
	// T10=P1P0 and T12=P1P2 first:
	KoPoint t10 = currentPoint() - p1;
	KoPoint t12 = p2 - p1;

	// Calculate distance squares:
	double dsqT10 = t10.x() * t10.x() + t10.y() * t10.y();
	double dsqT12 = t12.x() * t12.x() + t12.y() * t12.y();

	// We now calculate tan(a/2) where a is the angle between T10 and T12.
	// We benefit from the facts T10*T12 = |T10|*|T12|*cos(a),
	// |T10xT12| = |T10|*|T12|*sin(a) (cross product) and tan(a/2) = sin(a)/[1-cos(a)].
	double num   = t10.x() * t12.y() - t10.y() * t12.x();
	double denom =
		sqrt( dsqT10 * dsqT12 )
			- t10.x() * t12.x()
			+ t10.y() * t12.y();

	if( 1.0 + denom == 1.0 )	// points are co-linear
		lineTo( p1 );	// just add a line to first point
    else
    {
		// |P1B0| = |P1B3| = r * tan(a/2):
		double dP1B0 = fabs( r * num / denom );

		// B0 = P1 + |P1B0| * T10/|T10|:
		KoPoint b0 = p1 + t10 * ( dP1B0 / sqrt( dsqT10 ) );

		// If B0 deviates from current point P0, add a line to it:
		if( !b0.isNear( currentPoint(), VGlobal::isNearRange ) )
			lineTo( b0 );

		// B3 = P1 + |P1B3| * T12/|T12|:
		KoPoint b3 = p1 + t12 * ( dP1B0 / sqrt( dsqT12 ) );


		// The two bezier-control points are located on the tangents at a fraction
		// of the distance [tangent points<->tangent intersection].
		double distsq =
			( p1.x() - b0.x() ) * ( p1.x() - b0.x() ) +
			( p1.y() - b0.y() ) * ( p1.y() - b0.y() );
		double rsq = r * r;
		double fract;

		if( distsq >= rsq * VGlobal::veryBigNumber ) // r is very small
			fract = 0.0; // dist==r==0
		else
			fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ) );

		KoPoint b1 = b0 + ( p1 - b0 ) * fract;
		KoPoint b2 = b3 + ( p1 - b3 ) * fract;

		// Finally add the bezier-segment:
		curveTo( b1, b2, b3 );
	}

	return true;
}

void
VPath::close()
{
	// In the case the list is empty (which should actually never happen),
	// append a "begin" first, to avoid a crash:
	if( count() == 0 )
		append( new VSegment() );

	// Move end-segment if we are already closed:
	if( m_isClosed )
	{
		getLast()->setKnot( getFirst()->knot() );
	}
	// Append a line, if necessary:
	else
	{
		if(
			getLast()->knot().isNear(
				getFirst()->knot(), VGlobal::isNearRange ) )
		{
			// Move last knot:
			getLast()->setKnot( getFirst()->knot() );
		}
		else
		{
			// Add a line:
			lineTo( getFirst()->knot() );
		}

		m_isClosed = true;
	}
}

bool
VPath::counterClockwise() const
{
	// The idea for this algorithm is taken from the FAQ of comp.graphics.algorithms:
	// "Find the lowest vertex (or, if there is more than one vertex with the
	// same lowest coordinate, the rightmost of those vertices) and then take
	// the cross product of the edges fore and aft of it."

	if( !m_isClosed || m_number <= 1 )
		return false;

	VSegment* segment = m_first;

	// We save the segment not the knot itself. Initialize it with the
	// first segment:
	const VSegment* bottomRight = m_first;

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

		segment = segment->m_next;
	}


	// Catch boundary case (bottomRight is first or last segment):
	const VSegment* current;
	const VSegment* next;

	if( bottomRight == m_first )
		current = m_last;
	else
		current = bottomRight;

	if( bottomRight == m_last )
		next = m_first->next();
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
VPath::revert()
{
	if(!m_last)
		return;
	VPath list( parent() );
	list.moveTo( m_last->knot() );

	VSegment* segment = m_last;
	while( segment->m_prev )
	{
		list.append( segment->revert() );
		segment = segment->m_prev;
	}

	list.m_isClosed = m_isClosed;

	*this = list;
}

const KoRect&
VPath::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		VSegment* segment = m_first;

		while( segment )
		{
			m_boundingBox |= segment->boundingBox();
			segment = segment->m_next;
		}

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

VPath*
VPath::clone() const
{
	return new VPath( *this );
}

void
VPath::transform( const QWMatrix& m )
{
	VSegment* segment = m_first;
	while( segment )
	{
		segment->m_node[0] = segment->m_node[0].transform( m );
		segment->m_node[1] = segment->m_node[1].transform( m );
		segment->m_node[2] = segment->m_node[2].transform( m );

		segment = segment->m_next;
	}

	invalidateBoundingBox();
}

void
VPath::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "PATH" );
	element.appendChild( me );

	if( m_isClosed )
		me.setAttribute( "isClosed", m_isClosed );

	// save segments:
	VSegment* segment = m_first;
	while( segment )
	{
		segment->save( me );

		segment = segment->m_next;
	}
}

void
VPath::load( const QDomElement& element )
{
	clear();	// we already might have a "begin".

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
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
VPath::accept( VVisitor& visitor )
{
	visitor.visitVPath( *this );
}


VPath&
VPath::operator=( const VPath& list )
{
	if( this == &list )
		return *this;

	m_isClosed = list.m_isClosed;

	clear();

	VSegment* segment = list.m_first;
	while ( segment )
	{
		append( segment->clone() );
		segment = segment->m_next;
	}

	m_current = m_first;
	m_currentIndex = 0;

	return *this;
}

bool
VPath::insert( const VSegment* segment )
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
VPath::insert( uint index, const VSegment* segment )
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
VPath::prepend( const VSegment* segment )
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
VPath::append( const VSegment* segment )
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
VPath::clear()
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
VPath::first()
{
	if( m_first )
	{
		m_currentIndex = 0;
		return m_current = m_first;
	}

	return 0L;
}

VSegment*
VPath::last()
{
	if( m_last )
	{
		m_currentIndex = m_number - 1;
		return m_current = m_last;
	}

	return 0L;
}

VSegment*
VPath::prev()
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
VPath::next()
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
VPath::locate( uint index )
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
		while ( distance-- )
			segment = segment->m_next;
	}
	else
	{
		while ( distance-- )
			segment = segment->m_prev;
	}

	m_currentIndex = index;
	return m_current = segment;
}


VPathIterator::VPathIterator( const VPath& list )
{
	m_list = const_cast<VPath*>( &list );
	m_current = m_list->m_first;

	if( !m_list->m_iteratorList )
		m_list->m_iteratorList = new VPathIteratorList();

	m_list->m_iteratorList->add( this );
}

VPathIterator::VPathIterator( const VPathIterator& itr )
{
	m_list = itr.m_list;
	m_current = itr.m_current;

	if( m_list )
		m_list->m_iteratorList->add( this );
}

VPathIterator::~VPathIterator()
{
	if( m_list )
		m_list->m_iteratorList->remove( this );
}

VPathIterator&
VPathIterator::operator=( const VPathIterator& itr )
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
VPathIterator::operator()()
{
	if( m_current )
	{
		VSegment* const old = m_current;
		m_current = m_current->m_next;
		return old;
	}

	return 0L;
}

VSegment*
VPathIterator::operator++()
{
	if( m_current )
		return m_current = m_current->m_next;

	return 0L;
}

VSegment*
VPathIterator::operator+=( uint i )
{
	while ( m_current && i-- )
		m_current = m_current->m_next;

	return m_current;
}

VSegment*
VPathIterator::operator--()
{
	if( m_current )
		return m_current = m_current->m_prev;

	return 0L;
}

VSegment*
VPathIterator::operator-=( uint i )
{
	while ( m_current && i-- )
		m_current = m_current->m_prev;

	return m_current;
}

