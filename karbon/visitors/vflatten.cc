/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qptrlist.h>

#include "vflatten.h"
#include "vsegment.h"
#include "vsegmentlist.h"

void
VFlatten::setFlatness( double flatness )
{
	if( flatness > 0.0 )
		m_flatness = flatness;
}

void
VFlatten::visitVSegmentList( VSegmentList& segmentList )
{
	segmentList.first();

	// ommit "begin" segment:
	while( segmentList.next() )
	{
		while( !segmentList.current()->isFlat( m_flatness )  )
		{
			// split at midpoint:
			segmentList.insert(
				segmentList.current()->splitAt( 0.5 ) );
		}

		// convert to line:
		segmentList.current()->setType( VSegment::line );

		if( !success() )
			setSuccess();
	}
}

