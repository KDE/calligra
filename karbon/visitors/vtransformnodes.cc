/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
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

