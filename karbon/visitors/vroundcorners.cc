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
	// We'll change segments from segmentList. that doesnt hurt, since we
	// replace segmentList with newList afterwards.

	// Temporary list:
	VSegmentList newList( 0L );

	segmentList.first();
	// skip "begin":
	segmentList.next();


	double length;
	double param;

	// begin:
	if(
		segmentList.isClosed() &&
		!(
			segmentList.current()->type() == segment_curve &&
			segmentList.getLast()->type() == segment_curve ) )
	{
		length = segmentList.current()->length();

		param = length > 2 * m_radius
			? segmentList.current()->param( m_radius )
			: param = 0.5;


		segmentList.insert(
			segmentList.current()->splitAt( param ) );

		newList.moveTo(
			segmentList.current()->knot() );

		segmentList.next();
	}
	else
	{
		newList.moveTo(
			segmentList.current()->prev()->knot() );
	}


	// middle part:
	while(
		segmentList.current() &&
		segmentList.current()->next() )
	{
		if(
			!(
				segmentList.current()->type() == segment_curve &&
				segmentList.current()->next()->type() == segment_curve ) )
		{
			length = segmentList.current()->length();

			param = length > 2 * m_radius
				? segmentList.current()->param( length - m_radius )
				: param = 0.5;

			segmentList.insert(
				segmentList.current()->splitAt( param ) );
			newList.append(
				segmentList.current()->clone() );
			segmentList.next();


			segmentList.next();


			length = segmentList.current()->length();

			param = length > 2 * m_radius
				? segmentList.current()->param( m_radius )
				: param = 0.5;

			segmentList.insert(
				segmentList.current()->splitAt( param ) );


			newList.curveTo(
				segmentList.current()->prev()->prev()->knot() +
					0.5 * m_radius * segmentList.current()->prev()->tangent( 0.0 ),
				segmentList.current()->knot() -
					0.5 * m_radius * segmentList.current()->tangent( 1.0 ),
				segmentList.current()->knot() );
		}
		else
			newList.append( segmentList.current()->clone() );

		segmentList.next();
	}


	// end:
	if( segmentList.isClosed() )
	{
		if(
			!(
				segmentList.current()->type() == segment_curve &&
				segmentList.getFirst()->next()->type() == segment_curve ) )
		{
			length = segmentList.current()->length();

			param = length > 2 * m_radius
				? segmentList.current()->param( length - m_radius )
				: param = 0.5;

			segmentList.insert(
				segmentList.current()->splitAt( param ) );
			newList.append(
				segmentList.current()->clone() );
			segmentList.next();


			segmentList.first();
			segmentList.next();

			newList.curveTo(
				segmentList.getLast()->prev()->knot() +
					0.5 * m_radius * segmentList.getLast()->tangent( 0.0 ),
				segmentList.current()->knot() -
					0.5 * m_radius * segmentList.current()->tangent( 1.0 ),
				segmentList.current()->knot() );
		}
		else
			newList.append( segmentList.current()->clone() );

		newList.close();
	}
	else
		newList.append( segmentList.current()->clone() );


	segmentList = newList;

	// invalidate bounding box once:
	segmentList.invalidateBoundingBox();
}

