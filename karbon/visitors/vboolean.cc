/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qptrlist.h>
#include <qvaluelist.h>

#include "vboolean.h"
#include "vsegment.h"
#include "vsegmentlist.h"


bool
VBoolean::visit( VObject& object1, VObject& object2 )
{
	m_list1 = 0L;
	m_list2 = 0L;
	object1.accept( *this );
	object2.accept( *this );

	return success();
}

void
VBoolean::visitVSegmentList( VSegmentList& segmentList )
{
	if( m_list1 == 0L )
		m_list1 = &segmentList;
	else if( m_list2 == 0L )
	{
		m_list2 = &segmentList;
		doIt();
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
			KoPoint v1  = segment1.knot() - segment1.prev()->knot();
			KoPoint v2  = segment2.knot() - segment2.prev()->knot();

			double det = v2.x() * v1.y() - v2.y() * v1.x();

			if( 1.0 + det == 1.0 )
				return;
			else
			{
				KoPoint v = segment2.prev()->knot() - segment1.prev()->knot();
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
			VSegmentList list2( segment2 );
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
		VSegmentList list1( segment1 );
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
			VSegmentList list2( segment2 );
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

void
VBoolean::doIt()
{
	if( m_list1 == 0L || m_list2 == 0L )
		return;

	// intersection parameters (t):
	VParamList params1;
	VParamList params2;
	VParamList::iterator pItr;

	double prevParam;

	m_list1->first();

	// ommit "begin" segment:
	while( m_list1->next() )
	{
		params1.clear();

		m_list2->first();

		// ommit "begin" segment:
		while( m_list2->next() )
		{
			params2.clear();
		
			recursiveSubdivision(
				*m_list1->current(), 0.0, 1.0,
				*m_list2->current(), 0.0, 1.0,
				params1, params2 );

			qHeapSort( params2 );

			prevParam = 0.0;

			// walk down all intersection params and insert knots:
			for( pItr = params2.begin(); pItr != params2.end(); ++pItr )
			{
				m_list2->insert(
					m_list2->current()->splitAt(
						( *pItr - prevParam )/( 1.0 - prevParam ) ) );

				m_list2->next();
				prevParam = *pItr;
			}
		}

		qHeapSort( params1 );

		prevParam = 0.0;

		// walk down all intersection params and insert knots:
		for( pItr = params1.begin(); pItr != params1.end(); ++pItr )
		{
			m_list1->insert(
				m_list1->current()->splitAt(
					( *pItr - prevParam )/( 1.0 - prevParam ) ) );

			m_list1->next();
			prevParam = *pItr;
		}
	}
}

