/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qdom.h>
#include <qvaluelist.h>
#include <qwmatrix.h>

#include "vsegment.h"
#include "vsegmentlist.h"
#include "vvisitor.h"

#include <kdebug.h>


class VSegmentListIteratorList
{
public:
	VSegmentListIteratorList()
		: m_list( 0L ), m_iterator( 0L ) {}

	~VSegmentListIteratorList()
	{
		notifyClear( true );
		delete m_list;
	}

	void add( VSegmentListIterator* itr )
	{
		if( !m_iterator )
			m_iterator = itr;
		else if( m_list )
			m_list->push_front( itr );
		else
		{
			m_list = new QValueList<VSegmentListIterator*>;
			m_list->push_front( itr );
		}
	}

	void remove( VSegmentListIterator* itr ) {
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
				QValueList<VSegmentListIterator*>::Iterator itr = m_list->begin();
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
				QValueList<VSegmentListIterator*>::Iterator itr = m_list->begin();
				itr != m_list->end();
				++itr )
			{
			if( ( *itr )->m_current == segment )
				( *itr )->m_current = current;
			}
		}
	}

private:
	QValueList<VSegmentListIterator*>* m_list;
	VSegmentListIterator* m_iterator;
};


VSegmentList::VSegmentList( VObject* parent )
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

VSegmentList::VSegmentList( const VSegmentList& list )
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
		append( new VSegment( *segment ) );
		segment = segment->m_next;
	}
}

VSegmentList::~VSegmentList()
{
	clear();
	delete m_iteratorList;
}

const KoPoint&
VSegmentList::currentPoint() const
{
	return getLast()->knot2();
}

bool
VSegmentList::moveTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	// move "begin" when path is still empty:
	if( getLast()->type() == segment_begin )
	{
		getLast()->setKnot2( p );
		return true;
	}

	return false;
}

bool
VSegmentList::lineTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment();
	s->setType( segment_line );
	s->setKnot2( p );
	append( s );

	return true;
}

bool
VSegmentList::curveTo(
	const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment();
	s->setType( segment_curve );
	s->setCtrlPoint1( p1 );
	s->setCtrlPoint2( p2 );
	s->setKnot2( p3 );
	append( s );

	return true;
}

bool
VSegmentList::curve1To( const KoPoint& p2, const KoPoint& p3 )
{
	if( isClosed() ) return false;

	VSegment* s = new VSegment();
	s->setType( segment_curve1 );
	s->setCtrlPoint2( p2 );
	s->setKnot2( p3 );
	append( s );

	return true;
}

bool
VSegmentList::curve2To( const KoPoint& p1, const KoPoint& p3 )
{
	if( isClosed() );

	VSegment* s = new VSegment();
	s->setType( segment_curve2 );
	s->setCtrlPoint1( p1 );
	s->setKnot2( p3 );
	append( s );

	return true;
}

bool
VSegmentList::arcTo(
	const KoPoint& p1, const KoPoint& p2, const double r )
{
	// this routine is inspired by GNU ghostscript.

	if( isClosed() ) return false;

	// we need to calculate the tangent points. therefore calculate tangents
	// D10=P1P0 and D12=P1P2 first:
	KoPoint d10 = currentPoint() - p1;
	KoPoint d12 = p2 - p1;

	// calculate distance squares:
	double dsq10 = d10.x()*d10.x() + d10.y()*d10.y();
	double dsq12 = d12.x()*d12.x() + d12.y()*d12.y();

	// we now calculate tan(a/2) where a is the angular between D10 and D12.
	// we take advantage of D10*D12=d10*d12*cos(a), |D10xD12|=d10*d12*sin(a)
	// (cross product) and tan(a/2)=sin(a)/[1-cos(a)].
	double num   = d10.x() * d12.y() - d10.y() * d12.x();
	double denom =
		sqrt( dsq10 * dsq12 )
		- d10.x() * d12.x()
		+ d10.y() * d12.y();

	if( 1.0 + denom == 1.0 )	// points are co-linear
		lineTo( p1 );	// just add a line to first point
    else
    {
		// calculate distances from P1 to tangent points:
		double dist = fabs( r*num / denom );
		double d1t0 = dist / sqrt(dsq10);
		double d1t1 = dist / sqrt(dsq12);

// TODO: check for r<0

		KoPoint b0 = p1 + d10 * d1t0;

		// if b0 deviates from current point, add a line to it:
// TODO: decide via radius<XXX or sthg?
		if( b0 !=  currentPoint() )
			lineTo( b0 );

		KoPoint b3 = p1 + d12 * d1t1;

		// the two bezier-control points are located on the tangents at a fraction
		// of the distance [tangent points<->tangent intersection].
		double distsq =
			( p1.x() - b0.x() )*( p1.x() - b0.x() ) +
			( p1.y() - b0.y() )*( p1.y() - b0.y() );
		double rsq = r*r;
		double fract;

// TODO: make this nicer?

		if( distsq >= rsq * 1.0e8 ) // r is very small
			fract = 0.0; // dist==r==0
		else
			fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ) );

		KoPoint b1 = b0 + ( p1 - b0 ) * fract;
		KoPoint b2 = b3 + ( p1 - b3 ) * fract;

		// finally add the bezier-segment:
		curveTo( b1, b2, b3 );
	}

	return true;
}

void
VSegmentList::close()
{
	// move end-segment if one already exists:
	if( getLast()->type() == segment_end )
	{
		getLast()->setKnot2( getFirst()->knot2() );
	}
	// append one, if no end-segment exists:
	else if( getLast()->knot2() != getFirst()->knot2() )
	{
		VSegment* s = new VSegment();
		s->setType( segment_end );
		s->setKnot2( getFirst()->knot2() );
		append( s );
	}

	m_isClosed = true;
}

const KoRect&
VSegmentList::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		VSegment* segment = m_first;

		// ommit "begin" segment:
		while( ( segment = segment->m_next ) )
		{
			m_boundingBox |= segment->boundingBox();
		}

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

VSegmentList*
VSegmentList::clone() const
{
	return new VSegmentList( *this );
}

void
VSegmentList::transform( const QWMatrix& m )
{
	VSegment* segment = m_first;
	while( segment )
	{
		segment->setCtrlPoint1( segment->ctrlPoint1().transform( m ) );
		segment->setCtrlPoint2( segment->ctrlPoint2().transform( m ) );
		segment->setKnot2( segment->knot2().transform( m ) );

		segment = segment->m_next;
	}

	invalidateBoundingBox();
}

void
VSegmentList::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "SEGMENTS" );
	element.appendChild( me );

	if( m_isClosed )
		me.setAttribute( "isClosed", m_isClosed );

// TODO: optimize this since we have direct access now:
	// save segments:
	VSegmentListIterator itr( *this );
	for( ; itr.current() ; ++itr )
	{
		itr.current()->save( me );
	}
}

void
VSegmentList::load( const QDomElement& element )
{
	clear();	// we already have a "begin".

	m_isClosed = element.attribute( "isClosed" ) == 0 ? false : true;

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

	// "end"s do not get saved:
	if( m_isClosed )
		close();
}

void
VSegmentList::accept( VVisitor& visitor )
{
	visitor.visitVSegmentList( *this );
}


VSegmentList&
VSegmentList::operator=( const VSegmentList& list )
{
	if( &list == this )
		return *this;

	clear();
	if( list.count() > 0 )
	{
		VSegment* segment = list.m_first;
		while ( segment )
		{
			append( segment );
			segment = segment->m_next;
		}

		m_current = m_first;
		m_currentIndex = 0;
	}

	return *this;
}

bool
VSegmentList::insert( const VSegment* segment )
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
VSegmentList::insert( uint index, const VSegment* segment )
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
VSegmentList::prepend( const VSegment* segment )
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
VSegmentList::append( const VSegment* segment )
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
VSegmentList::clear()
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

	invalidateBoundingBox();
}

VSegment*
VSegmentList::first()
{
	if( m_first )
	{
		m_currentIndex = 0;
		return m_current = m_first;
	}

	return 0L;
}

VSegment*
VSegmentList::last()
{
	if( m_last )
	{
		m_currentIndex = m_number - 1;
		return m_current = m_last;
	}

	return 0L;
}

VSegment*
VSegmentList::prev()
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
VSegmentList::next()
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
VSegmentList::locate( uint index )
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


VSegmentListIterator::VSegmentListIterator( const VSegmentList& list )
{
	m_list = const_cast<VSegmentList*>( &list );
	m_current = m_list->m_first;

	if( !m_list->m_iteratorList )
		m_list->m_iteratorList = new VSegmentListIteratorList();

	m_list->m_iteratorList->add( this );
}

VSegmentListIterator::VSegmentListIterator( const VSegmentListIterator& itr )
{
	m_list = itr.m_list;
	m_current = itr.m_current;

	if( m_list )
		m_list->m_iteratorList->add( this );
}

VSegmentListIterator::~VSegmentListIterator()
{
	if( m_list )
		m_list->m_iteratorList->remove( this );
}

VSegmentListIterator&
VSegmentListIterator::operator=( const VSegmentListIterator& itr )
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
VSegmentListIterator::operator()()
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
VSegmentListIterator::operator++()
{
	if( m_current )
		return m_current = m_current->m_next;

	return 0L;
}

VSegment*
VSegmentListIterator::operator+=( uint i )
{
	while ( m_current && i-- )
		m_current = m_current->m_next;

	return m_current;
}

VSegment*
VSegmentListIterator::operator--()
{
	if( m_current )
		return m_current = m_current->m_prev;

	return 0L;
}

VSegment*
VSegmentListIterator::operator-=( uint i )
{
	while ( m_current && i-- )
		m_current = m_current->m_prev;

	return m_current;
}

