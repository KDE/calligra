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

	// ommit "begin" segment:
	while(
		segmentList.next() && 
		segmentList.current()->next() )
	{
		newList.arcTo(
			segmentList.current()->knot2(),
			segmentList.current()->next()->knot2(),
			m_radius );
	}

	segmentList = newList;
}

