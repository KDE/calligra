/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vboolean.h"
#include "vpath.h"

void
VBoolean::visit( VObject& object1, VObject& object2 )
{
	m_lists1 = 0L;
	m_lists2 = 0L;
	object1.accept( *this );
	object2.accept( *this );
}

void
VBoolean::visitVPath(
	VPath& /*path*/, QPtrList<VSegmentList>& lists )
{
	if( m_lists1 == 0L )
		m_lists1 = &lists;
	else if( m_lists2 == 0L )
	{
		m_lists2 = &lists;
		doIt();
	}
}

void
VBoolean::doIt()
{
	if( m_lists1 == 0L || m_lists2 == 0L )
		return;

	QPtrListIterator<VSegmentList> itr1( *m_lists1 );
	QPtrListIterator<VSegmentList> itr2( *m_lists2 );

	// copies for detecting intersections:
	VSegment segment1;
	VSegment segment2;

	// test each segment with all others:
	for( ; itr1.current(); ++itr1 )
	{
		itr1.current()->first();

		// ommit "begin" segment:
		while( itr1.current()->next() )
		{
			segment1 = *itr1.current()->next();

			for( ; itr2.current(); ++itr2 )
			{
				itr2.current()->first();

				// ommit "begin" segment:
				while( itr2.current()->next() )
				{
					segment2 = *itr2.current()->next();

					m_params.clear();

				}
			}
		}
	}
}

void
VBoolean::recursiveSubdivision()
{
}

