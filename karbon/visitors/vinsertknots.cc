/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qptrlist.h>

#include "vinsertknots.h"
#include "vsegment.h"
#include "vsegmentlist.h"


void
VInsertKnots::setKnots( uint knots )
{
	if( knots )
		m_knots = knots;
}

void
VInsertKnots::visitVSegmentList( VSegmentList& segmentList )
{
	segmentList.first();

	// ommit "begin" segment:
	while( segmentList.next() )
	{
		for( uint i = m_knots; i > 0; --i )
		{
			segmentList.insert(
				segmentList.current()->splitAt( 1.0 / ( i + 1.0 ) ) );

			segmentList.next();
		}

		if( !success() )
			setSuccess();
	}
}

