/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpath.h"
#include "vpolygonize.h"
#include "vsegment.h"

void
VPolygonize::setFlatness( double flatness )
{
	if( flatness > 0.0 )
		m_flatness = flatness;
}

void
VPolygonize::visitVPath(
	VPath& /*path*/, QPtrList<VSegmentList>& lists )
{
	QPtrListIterator<VSegmentList> itr( lists );
	for( ; itr.current(); ++itr )
	{
		itr.current()->first();

		// ommit "begin" segment:
		while( itr.current()->next() )
		{
			while( !itr.current()->current()->isFlat( m_flatness )  )
			{
				// split at midpoint:
				itr.current()->insert(
					itr.current()->current()->splitAt( 0.5 ) );
			}

			// convert to line:
			itr.current()->current()->setType( segment_line );
		}
	}
}

