/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vsegment.h"
#include "vsegmentlist.h"
#include "vselectnodes.h"

void
VSelectNodes::visitVSegmentList( VSegmentList& segmentList )
{
	segmentList.first();

	// skip "begin":
	while( segmentList.next() )
	{
		if( m_deselect )
			segmentList.current()->deselect();
		else if( !m_point.isNull() )
		{
			if( segmentList.current()->nodeNear( m_point, 2.0 ) )
			{
				m_segments.append( segmentList.current() );
			}
		}
		else
		{
			if( segmentList.current()->select( m_rect ) )
			{
				m_segments.append( segmentList.current() );
			}
		}
	}
}

