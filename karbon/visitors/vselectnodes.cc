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
		if( m_rect.isEmpty() )
		{
			segmentList.current()->selectCtrlPoint1( m_select );
			segmentList.current()->selectCtrlPoint2( m_select );
			segmentList.current()->selectKnot( m_select );

			setSuccess();
		}
		else
		{
			if(
				segmentList.current()->ctrlPointFixing() != VSegment::first &&
				m_rect.contains( segmentList.current()->ctrlPoint1() ) )
			{
				// select first control point, when previous knot is selected:
				if(
					segmentList.current()->prev() &&
					segmentList.current()->prev()->knotSelected() )
				{
					segmentList.current()->selectCtrlPoint1( m_select );

					setSuccess();
				}
			}

			if(
				segmentList.current()->ctrlPointFixing() != VSegment::second &&
				m_rect.contains( segmentList.current()->ctrlPoint2() ) )
			{
				// select second control point, when knot is selected:
				if( segmentList.current()->knotSelected() )
				{
					segmentList.current()->selectCtrlPoint2( m_select );

					setSuccess();
				}
			}

			if( m_rect.contains( segmentList.current()->knot() ) )
			{
				segmentList.current()->selectKnot( m_select );

				setSuccess();
			}
		}
	}
}

