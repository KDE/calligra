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
	if( segmentList.isClosed() )
	{
		newList.moveTo(
			segmentList.current()->next()->point(
				parameter( *segmentList.current()->next() ) ) );
	}
	else
	{
		newList.moveTo(
			segmentList.current()->knot2() );
	}


	// middle part:
	while(
		segmentList.next() && 
		segmentList.current()->next() )
	{
		roundCorner(
			*segmentList.current(),
			*segmentList.current()->next(),
			newList );
	}


	// end:
	if( segmentList.isClosed() )
	{
		roundCorner(
			*segmentList.current(),
			*segmentList.getFirst()->next(),
			newList );

		newList.close();
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
			? m_radius / segment.length()
			: 0.5;
}

void
VRoundCorners::roundCorner(
	const VSegment& current, const VSegment& next, VSegmentList& segmentList ) const
{
	double t1 = parameter( current );
	double t2 = parameter( next );

	segmentList.lineTo(
		current.point( 1.0 - t1 ) );

	// robust concerning to small segments:
	segmentList.curveTo(
		current.point( 1.0 - 0.5 * t1 ),
		next.point( 0.5 * t2 ),
		next.point( t2 ) );
}

