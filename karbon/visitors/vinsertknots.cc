/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vinsertknots.h"
#include "vpath.h"
#include "vsegment.h"

void
VInsertKnots::setKnots( uint knots )
{
	if( knots )
		m_knots = knots;
}

void
VInsertKnots::visitVPath(
	VPath& /*path*/, QPtrList<VSegmentList>& lists )
{
	QPtrListIterator<VSegmentList> itr( lists );
	for( ; itr.current(); ++itr )
	{
		itr.current()->first();

		// ommit "begin" segment:
		while( itr.current()->next() )
		{
			for( uint i = m_knots; i > 0; --i )
			{
				itr.current()->insert(
					itr.current()->current()->splitAt( 1.0 / ( i + 1.0 ) ) );
				itr.current()->next();
			}
		}
	}
}

