/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vsegment.h"

VSegmentListTraverser::VSegmentListTraverser()
	:  m_previousPoint( 0.0, 0.0 )
{
}

bool
VSegmentListTraverser::traverse( const VSegmentList& list )
{
	VSegmentListIterator itr( list );
	for( ; itr.current(); ++itr )
	{
		if( itr.current()->type() == VSegment::curve )
		{
			if( !curveTo(
				itr.current()->point( 1 ),
				itr.current()->point( 2 ),
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::line )
		{
			if( !lineTo(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::curve1 )
		{
			if( !curve1To(
				itr.current()->point( 2 ),
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::curve2 )
		{
			if( !curve2To(
				itr.current()->point( 1 ),
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::begin )
		{
			if( !begin(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		// fall back:
		else
		{
			if( !lineTo(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
	}

	return true;
}


VSegment::VSegment()
	: m_type( begin )
{
}

VSegment::VSegment( const VSegment& other )
{
	m_point[0] = other.m_point[0];
	m_point[1] = other.m_point[1];
	m_point[2] = other.m_point[2];
	m_type = other.m_type;
}
