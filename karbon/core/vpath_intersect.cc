/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpath.h"
#include "vpath_intersect.h"
#include "vsegment_tools.h"

#include <kdebug.h>

VPathIntersect::VPathIntersect()
	: VSegmentListTraverser()
{
}

VSegmentList*
VPathIntersect::intersect( const VSegmentList& list1, const VSegmentList& list2 )
{
	m_list2 = const_cast<VSegmentList*>( &list2 );

	m_isList1 = true;
	traverse( list1 );

	return 0L;
}

bool
VPathIntersect::begin( const KoPoint& p,
	const VSegment& segment )
{
	if( m_isList1 )
	{
		m_segment1 = 0L;
		setPreviousPoint( p );
	}
	else
	{
		m_segment2 = 0L;
		setPreviousPoint2( p );
	}

	return true;
}

bool
VPathIntersect::curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3,
	const VSegment& segment )
{
	if( m_isList1 )
	{
		m_segment1 = &segment;

		m_isList1 = false;
		traverse( *m_list2 );
		m_isList1 = true;

		setPreviousPoint( p3 );
	}
	else
	{
		m_segment2 = &segment;

		setPreviousPoint2( p3 );
	}

	return true;
}

bool
VPathIntersect::curve1To( const KoPoint& p2, const KoPoint& p3,
	const VSegment& segment )
{
	if( m_isList1 )
	{
		m_isList1 = false;
		traverse( *m_list2 );
		m_isList1 = true;

		setPreviousPoint( p3 );
	}
	else
	{
		setPreviousPoint2( p3 );
	}

	return true;
}

bool
VPathIntersect::curve2To( const KoPoint& p1, const KoPoint& p3,
	const VSegment& segment )
{
	if( m_isList1 )
	{
		m_isList1 = false;
		traverse( *m_list2 );
		m_isList1 = true;

		setPreviousPoint( p3 );
	}
	else
	{
		setPreviousPoint2( p3 );
	}

	return true;
}


bool
VPathIntersect::lineTo( const KoPoint& p,
	const VSegment& segment )
{
	if( m_isList1 )
	{
		m_isList1 = false;
		traverse( *m_list2 );
		m_isList1 = true;

		setPreviousPoint( p );
	}
	else
	{
		setPreviousPoint2( p );
	}

	return true;
}

bool
VPathIntersect::end( const KoPoint& p,
	const VSegment& segment )
{
	if( m_isList1 )
	{
		m_isList1 = false;
		traverse( *m_list2 );
		m_isList1 = true;

		setPreviousPoint( p );
	}
	else
	{
		setPreviousPoint2( p );
	}

	return true;
}
