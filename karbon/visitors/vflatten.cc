/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qptrlist.h>

#include "vflatten.h"
#include "vsegment.h"
#include "vsegmentlist.h"


// TODO: Think about if we want to adapt this:

/*
 * <cite from GNU ghostscript's gxpflat.c>
 *
 * To calculate how many points to sample along a path in order to
 * approximate it to the desired degree of flatness, we define
 *      dist((x,y)) = abs(x) + abs(y);
 * then the number of points we need is
 *      N = 1 + sqrt(3/4 * D / flatness),
 * where
 *      D = max(dist(p0 - 2*p1 + p2), dist(p1 - 2*p2 + p3)).
 * Since we are going to use a power of 2 for the number of intervals,
 * we can avoid the square root by letting
 *      N = 1 + 2^(ceiling(log2(3/4 * D / flatness) / 2)).
 * (Reference: DEC Paris Research Laboratory report #1, May 1989.)
 *
 * We treat two cases specially.  First, if the curve is very
 * short, we halve the flatness, to avoid turning short shallow curves
 * into short straight lines.  Second, if the curve forms part of a
 * character (indicated by flatness = 0), we let
 *      N = 1 + 2 * max(abs(x3-x0), abs(y3-y0)).
 * This is probably too conservative, but it produces good results.
 *
 * </cite from GNU ghostscript's gxpflat.c>
 */


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

