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
#include "vtransformnodes.h"


VTransformNodes::VTransformNodes( const QWMatrix& m )
	: m_matrix( m )
{
}

void
VTransformNodes::visitVSegmentList( VSegmentList& segmentList )
{
	segmentList.first();

	// skip "begin":
	while( segmentList.next() )
	{
		if(
			segmentList.current()->prev() &&
			segmentList.current()->prev()->edited( 2 ) )
		{
			// Do nothing.
		}
		else if( segmentList.current()->edited( 0 ) )
		{
			segmentList.current()->setCtrlPoint1(
				segmentList.current()->ctrlPoint1().transform( m_matrix ) );
		}

		if( segmentList.current()->edited( 2 ) )
		{
			segmentList.current()->setCtrlPoint2(
				segmentList.current()->ctrlPoint2().transform( m_matrix ) );
			segmentList.current()->setKnot(
				segmentList.current()->knot().transform( m_matrix ) );

			if( segmentList.current() == segmentList.getLast() )
			{
				segmentList.getFirst()->setKnot(
					segmentList.getFirst()->knot().transform( m_matrix ) );
				segmentList.getFirst()->next()->setCtrlPoint1(
					segmentList.getFirst()->next()->ctrlPoint1().transform( m_matrix ) );
			}
			else
			{
				segmentList.current()->next()->setCtrlPoint1(
					segmentList.current()->next()->ctrlPoint1().transform( m_matrix ) );
			}
		}
		else if( segmentList.current()->edited( 1 ) )
		{
			segmentList.current()->setCtrlPoint2(
				segmentList.current()->ctrlPoint2().transform( m_matrix ) );
		}

		if( !success() )
			setSuccess();
	}
}

