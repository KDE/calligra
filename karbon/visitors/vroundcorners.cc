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
	VSegmentList newList( 0L );
	newList.moveTo( segmentList.first()->knot2() );

	double t1;
	double t2;

	// ommit "begin" segment:
	while(
		segmentList.next() && 
		segmentList.current()->next() )
	{
		t1 = m_radius / segmentList.current()->length();
		t2 = m_radius / segmentList.current()->next()->length();

		newList.lineTo(
			segmentList.current()->point( 1.0 - t1 )
		);

		newList.curveTo(
			segmentList.current()->point( 1.0 - 0.5 * t1 ),
			segmentList.current()->next()->point( 0.5 * t2 ),
			segmentList.current()->next()->point( t2 )
		);
	}

	segmentList = newList;

	// invalidate bounding box once:
	segmentList.invalidateBoundingBox();
}

