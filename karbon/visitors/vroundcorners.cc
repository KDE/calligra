/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qptrlist.h>

#include "vpath.h"
#include "vroundcorners.h"
#include "vsegment.h"


void
VRoundCorners::setRadius( double radius )
{
	if( radius > 0.0 )
		m_radius = radius;
}

void
VRoundCorners::visitVPath( VPath& path )
{
	QPtrListIterator<VSegmentList> itr( path.segmentLists() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
VRoundCorners::visitVSegmentList( VSegmentList& segmentList )
{
	// temporary list:
	VSegmentList newList( 0L );


	segmentList.first();

	// begin:
	if(
		segmentList.isClosed() &&
		segmentList.current()->type() != segment_curve &&	// we never touch
		segmentList.getLast()->type() != segment_curve )	// bezier/bezier joins.
	{
		newList.moveTo(
			segmentList.current()->next()->point(
				parameter( *segmentList.current()->next() ) ) );
	}
	else
	{
		newList.moveTo(
			segmentList.current()->knot() );
	}


	// middle part:
	while(
		segmentList.next() && 
		segmentList.current()->next() )
	{
		if(
			segmentList.current()->type() != segment_curve &&
			segmentList.current()->next()->type() != segment_curve )
		{
			roundCorner(
				segmentList,
				*segmentList.current()->next(),
				newList );
		}
		else
		{
		}
	}


	// end:
	if( segmentList.isClosed() )
	{
		if(
			segmentList.current()->type() != segment_curve &&
			segmentList.getFirst()->type() != segment_curve )
		{
			roundCorner(
				segmentList,
				*segmentList.getFirst()->next(),
				newList );
		}
		else
		{
		}

		newList.close();
	}
	else
	{
		newList.append( segmentList.current()->clone() );
	}

	segmentList = newList;

	// invalidate bounding box once:
	segmentList.invalidateBoundingBox();
}

double
VRoundCorners::parameter( const VSegment& segment ) const
{
	return
		segment.length() > 2 * m_radius
// TODO: this very calculation isnt quite correct. it assumes uniform velocity:
			? m_radius / segment.length()
			: 0.5;
}

void
VRoundCorners::roundCorner(
	VSegmentList& segmentList, const VSegment& next, VSegmentList& newList ) const
{
	double t1 = parameter( *segmentList.current() );
	double t2 = parameter( next );

	newList.lineTo(
		segmentList.current()->point( 1.0 - t1 ) );

	// robust concerning to small segments:
	newList.curveTo(
		segmentList.current()->point( 1.0 - 0.5 * t1 ),
		next.point( 0.5 * t2 ),
		next.point( t2 ) );
}

