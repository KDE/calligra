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

