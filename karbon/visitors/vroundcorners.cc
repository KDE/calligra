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

#include "vroundcorners.h"
#include "vsegment.h"
#include "vsegmentlist.h"


void
VRoundCorners::setRadius( double radius )
{
	if( radius > 0.0 )
		m_radius = radius;
}

void
VRoundCorners::visitVSegmentList( VSegmentList& segmentList )
{
	// Note: we change segments from segmentList. that doesnt hurt, since we
	// replace segmentList with newList afterwards.

	// Temporary list:
	VSegmentList newList( 0L );

	segmentList.first();
	// skip "begin":
	segmentList.next();

	// Description of the algorithm:
	//
	// Let's assume segmentList is closed and contains segments which build
	// a rectangle:
	//
	//           3
	//    X------------X
	//    |            |
	//   4|            |2      (numbers mean the segments' order
	//    |            |        in segmentList. we neglect the "begin"
	//    X------------X        segment)
	//           1
	//
	// We want to round the corners with "radius" m_radius (the algorithm
	// doesn't really produce circular arcs with fixed radii).
	// We need to process three different steps:
	//
	// 1) Begin
	//    -----
	//    Split the first segment of segmentList (called "segmentList[1]" here)
	//    at parameter t and move newList to this new knot. While
	//
	//        t = segmentList[1]->param( m_radius )
	//
	//    as long as segmentList[1] isnt too small (smaller than 2 * m_radius).
	//    In this case we set t = 0.5.
	//
	//    segmentList:          newList:
	//
	//           3
	//    X------------X
	//    |            |
	//   4|            |2
	//    |            |
	//    X--X---------X        ...X
	//           1                     1
	//
	// 2) Loop
	//    ----
	//    This step is repeated for each following segment.
	//    Split the current segment segmentList[n] at parameter t
	//    and add the first subsegment to newList. While
	//
	//        t = segmentList[n]->param( segmentList[n]->length() - m_radius )
	//
	//    as long as segmentList[n] isnt too small (smaller than 2 * m_radius).
	//    In this case we set t = 0.5.
	//
// TODO: round corner.
	//
	//    segmentList:          newList:
	//
	//           3
	//    X------------X
	//    |            |
	//   4|            X2                    X
	//    |            |                    /.2
	//    X--X------X--X           X------X...
	//           1                     1
	//
	// 3) End
	//    ---
// TODO: end.
	//
	//    segmentList:          newList:
	//
	//           3                     5
	//    X--X------X--X        6 .X------X. 4
	//    |            |         /          \
	//   4X            X2     7 X            X 3
	//    |            |      8 .\          /
	//    X--X------X--X        ...X------X. 2
	//           1                     1
	//
	// Further details:
	//
	// - We dont touch bezier/bezier joins, that's senseless.


	double length;
	double param;

	// begin:
	if(
		segmentList.isClosed() &&
		!(
			segmentList.current()->type() == VSegment::curve &&
			segmentList.getLast()->type() == VSegment::curve ) )
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
				segmentList.current()->type() == VSegment::curve &&
				segmentList.current()->next()->type() == VSegment::curve ) )
		{
			length = segmentList.current()->length();

			param = length > 2 * m_radius
				? segmentList.current()->param( length - m_radius )
				: 0.5;

			segmentList.insert(
				segmentList.current()->splitAt( param ) );
			newList.append(
				segmentList.current()->clone() );
			segmentList.next();


			segmentList.next();


			length = segmentList.current()->length();

			param = length > 2 * m_radius
				? segmentList.current()->param( m_radius )
				: 0.5;

			segmentList.insert(
				segmentList.current()->splitAt( param ) );


			// Round corner:
			newList.curveTo(
				segmentList.current()->prev()->point( 0.5 ),
				segmentList.current()->point( 0.5 ),
				segmentList.current()->knot() );
		}
		else
			newList.append( segmentList.current()->clone() );


		if( !success() )
			setSuccess();


		segmentList.next();
	}


	// end:
	if( segmentList.isClosed() )
	{
		if(
			!(
				segmentList.current()->type() == VSegment::curve &&
				segmentList.getFirst()->next()->type() == VSegment::curve ) )
		{
			length = segmentList.current()->length();

			param = length > 2 * m_radius
				? segmentList.current()->param( length - m_radius )
				: 0.5;

			segmentList.insert(
				segmentList.current()->splitAt( param ) );
			newList.append(
				segmentList.current()->clone() );
			segmentList.next();


			segmentList.first();
			segmentList.next();

			// Round corner:
			newList.curveTo(
				segmentList.getLast()->point( 0.5 ),
				segmentList.current()->point( 0.5 ),
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

