/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vnodeselector.h"
#include "vsegment.h"
#include "vsegmentlist.h"

void
VNodeSelector::visitVSegmentList( VSegmentList& segmentList )
{
	segmentList.first();
	// skip "begin":
	do
	{
		if( m_all )
			segmentList.current()->selectNode();
		else if( !m_point.isNull() )
		{
			bool selected =	segmentList.current()->checkNode( m_point );
			if( selected )
				m_segments.append( segmentList.current() );
		}
		else
		{
			bool selected =	segmentList.current()->selectNode( m_rect );
			if( selected )
				m_segments.append( segmentList.current() );
		}
	}
	while( segmentList.next() );
}

