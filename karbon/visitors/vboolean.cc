/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qvaluelist.h>

#include "vboolean.h"
#include "vpath.h"
#include "vsegment.h"


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

	// intersection parameters (t):
	VParamList params1;
	VParamList params2;
	VParamList::iterator pItr;

	double prevParam;

	// test each segment with all others:
	for( ; itr1.current(); ++itr1 )
	{
		for( ; itr2.current(); ++itr2 )
		{
			itr1.current()->first();

			// ommit "begin" segment:
			while( itr1.current()->next() )
			{
				params1.clear();

				itr2.current()->first();

				// ommit "begin" segment:
				while( itr2.current()->next() )
				{
					params2.clear();
				
					recursiveSubdivision(
						*itr1.current()->current(), 0.0, 1.0,
						*itr2.current()->current(), 0.0, 1.0,
						params1, params2 );

					qHeapSort( params2 );

					prevParam = 0.0;

					// walk down all intersection params and insert knots:
					for( pItr = params2.begin(); pItr != params2.end(); ++pItr )
					{
						itr2.current()->insert(
							itr2.current()->current()->splitAt(
								( *pItr - prevParam )/( 1.0 - prevParam ) ) );

						itr2.current()->next();
						prevParam = *pItr;
					}
				}

				qHeapSort( params1 );

				prevParam = 0.0;

				// walk down all intersection params and insert knots:
				for( pItr = params1.begin(); pItr != params1.end(); ++pItr )
				{
					itr1.current()->insert(
						itr1.current()->current()->splitAt(
							( *pItr - prevParam )/( 1.0 - prevParam ) ) );

					itr1.current()->next();
					prevParam = *pItr;
				}
			}
		}
	}
}

void
VBoolean::recursiveSubdivision(
	const VSegment& segment1, double t0_1, double t1_1,
	const VSegment& segment2, double t0_2, double t1_2,
	VParamList& params1, VParamList& params2 )
{
	if(
		!segment1.boundingBox().intersects(
			segment2.boundingBox() ) )
	{
		return;
	}

	if( segment1.isFlat() )
	{
		// calculate intersection of line segments:
		if( segment2.isFlat() )
		{
			KoPoint v1  = segment1.knot2() - segment1.knot1();
			KoPoint v2  = segment2.knot2() - segment2.knot1();

			double det = v2.x() * v1.y() - v2.y() * v1.x();

			if( 1.0 + det == 1.0 )
				return;
			else
			{
				KoPoint v = segment2.knot1() - segment1.knot1();
				const double one_det = 1.0 / det;

				double t1 = one_det * ( v2.x() * v.y() - v2.y() * v.x() );
				double t2 = one_det * ( v1.x() * v.y() - v1.y() * v.x() );

				if ( t1 < 0.0 || t1 > 1.0 || t2 < 0.0 || t2 > 1.0 )
					return;

				params1.append( t0_1 + t1 * ( t1_1 - t0_1 ) );
				params2.append( t0_2 + t2 * ( t1_2 - t0_2 ) );
			}
		}
		else
		{
			// "copy segment" and split it at midpoint:
			VSegmentList list2;
			list2.moveTo( segment2.knot1() );
			list2.append( new VSegment( segment2 ) );
			list2.insert( list2.current()->splitAt( 0.5 ) );

			double mid2 = 0.5 * ( t0_2 + t1_2 );

			recursiveSubdivision(
				*list2.current(), t0_2, mid2,
				segment1,         t0_1, t1_1, params2, params1 );
			recursiveSubdivision(
				*list2.next(),    mid2, t1_2,
				segment1,         t0_1, t1_1, params2, params1 );
		}
	}
	else
	{
		// "copy segment" and split it at midpoint:
		VSegmentList list1;
		list1.moveTo( segment1.knot1() );
		list1.append( new VSegment( segment1 ) );
		list1.insert( list1.current()->splitAt( 0.5 ) );

		double mid1 = 0.5 * ( t0_1 + t1_1 );

		if( segment2.isFlat() )
		{
			recursiveSubdivision(
				*list1.current(), t0_1, mid1,
				segment2,         t0_2, t1_2, params1, params2 );
			recursiveSubdivision(
				*list1.next(),    mid1, t1_1,
				segment2,         t0_2, t1_2, params1, params2 );
		}
		else
		{
			// "copy segment" and split it at midpoint:
			VSegmentList list2;
			list2.moveTo( segment2.knot1() );
			list2.append( new VSegment( segment2 ) );
			list2.insert( list2.current()->splitAt( 0.5 ) );

			double mid2 = 0.5 * ( t0_2 + t1_2 );

			recursiveSubdivision(
				*list1.current(), t0_1, mid1,
				*list2.current(), t0_2, mid2, params1, params2 );
			recursiveSubdivision(
				*list1.next(),    mid1, t1_1,
				*list2.current(), t0_2, mid2, params1, params2 );

			recursiveSubdivision(
				*list1.prev(),    t0_1, mid1,
				*list2.next(),    mid2, t1_2, params1, params2 );
			recursiveSubdivision(
				*list1.next(),    mid1, t1_1,
				*list2.current(), mid2, t1_2, params1, params2 );
		}
	}
}

