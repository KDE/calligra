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
		if( m_allNodes )
		{
			segmentList.current()->selectCtrlPoint1( m_select );
			segmentList.current()->selectCtrlPoint2( m_select );
			segmentList.current()->selectKnot( m_select );

			if( !success() )
				setSuccess();
		}
		else if( !m_point.isNull() )
		{
			if(
				segmentList.current()->ctrlPointFixing() != VSegment::first &&
				segmentList.current()->ctrlPoint1().isNear( m_point, m_isNearRange ) )
			{
				// select first control point, when previous knot is selected:
				//if(
				//	m_prev &&
				//	m_prev->m_nodeSelected[2] )
				//{
					segmentList.current()->selectCtrlPoint1( m_select );
//					m_nodeEdited[0] = m_select;

					if( m_select )
					{
						segmentList.current()->prev()->selectKnot( false );
//						m_nodeEdited[1] = false;
//						m_nodeEdited[2] = false;
					}

					setSuccess();
				//}
			}

			if(
				segmentList.current()->ctrlPointFixing() != VSegment::second &&
				segmentList.current()->ctrlPoint2().isNear( m_point, m_isNearRange ) )
			{
				// select second control point, when knot is selected:
				//if( m_nodeSelected[2] )
				//{
					segmentList.current()->selectCtrlPoint2( m_select );
//					m_nodeEdited[1] = m_select;

					if( m_select )
					{
						segmentList.current()->prev()->selectKnot( false );
//						m_nodeEdited[0] = false;
//						m_nodeEdited[2] = false;
					}

					setSuccess();
				//}
			}

			if( segmentList.current()->knot().isNear( m_point, m_isNearRange ) )
			{
				segmentList.current()->selectCtrlPoint2( m_select );
				segmentList.current()->selectKnot( m_select );
//				m_nodeSelected[1] = m_nodeEdited[1] = m_select;
//				m_nodeSelected[2] = m_nodeEdited[2] = m_select;

				setSuccess();
			}
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
//				m_nodeSelected[2] = m_nodeEdited[2] = true;

				setSuccess();
			}
		}
	}
}

