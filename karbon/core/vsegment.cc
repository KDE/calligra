/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vsegment.h"

VSegmentListTraverser::VSegmentListTraverser()
	:  m_previousPoint( 0.0, 0.0 )
{
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

bool
VSegment::traverse( const VSegmentList& list, VSegmentListTraverser& traverser )
{
	VSegmentListIterator itr( list );
	for( ; itr.current(); ++itr )
	{
		if( itr.current()->m_type == curve )
		{
			if( !traverser.curveTo(
				itr.current()->m_point[0],
				itr.current()->m_point[1],
				itr.current()->m_point[2] ) )
			{
				return false;
			}
		}
		else if( itr.current()->m_type == line )
		{
			if( !traverser.lineTo(
				itr.current()->m_point[2] ) )
			{
				return false;
			}
		}
		else if( itr.current()->m_type == curve1 )
		{
			if( !traverser.curve1To(
				itr.current()->m_point[1],
				itr.current()->m_point[2] ) )
			{
				return false;
			}
		}
		else if( itr.current()->m_type == curve2 )
		{
			if( !traverser.curve2To(
				itr.current()->m_point[0],
				itr.current()->m_point[2] ) )
			{
				return false;
			}
		}
		else if( itr.current()->m_type == begin )
		{
			if( !traverser.begin(
				itr.current()->m_point[2] ) )
			{
				return false;
			}
		}
		// fall back:
		else
		{
			if( !traverser.lineTo(
				itr.current()->m_point[2] ) )
			{
				return false;
			}
		}
	}

	return true;
}

