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


#include <klocale.h>

#include "vpath.h"
#include "vroundcornerscmd.h"
#include "vsegment.h"
#include "vselection.h"


VRoundCornersCmd::VRoundCornersCmd( VDocument* doc, double radius )
	: VCommand( doc, i18n( "Round Corners" ) )
{
	m_selection = m_doc->selection()->clone();

	m_radius = radius > 0.0 ? radius : 1.0;
}

VRoundCornersCmd::~VRoundCornersCmd()
{
	delete( m_selection );
}

void
VRoundCornersCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );

	for ( ; itr.current() ; ++itr )
		visit( *itr.current() );
}

void
VRoundCornersCmd::unexecute()
{
}

void
VRoundCornersCmd::visitVPath( VPath& path )
{
	// Note: we change segments from path. that doesnt hurt, since we
	// replace path with newPath afterwards.

	// Temporary path:
	VPath newPath( 0L );

	path.first();
	// skip "begin":
	path.next();

	// Description of the algorithm:
	// -----------------------------
	//
	// Without restricting generality, let's assume the path is closed and contains segments
	// which build a rectangle.
	//
	//           3
	//    X------------X
	//    |            |
	//   4|            |2      (Numbers mean the segments' order
	//    |            |        in the path. We neglect the "begin"
	//    X------------X        segment here.)
	//           1
	//
	// We want to round the corners with "radius" m_radius. Note: the algorithm
	// doesn't really produce circular arcs, but that's ok since the algorithm
	// achieves nice looking results and can be applied to all kind of paths.
	// Note also, that this algorithm doesn't touch curve/curve joins, since they
	// are usually smooth enough.
	//
	// We'll manipulate the input path for bookkeeping purposes and construct a new path in 
	// parallel. We finally replace the input path with the new path.
	//
	// There are three unique steps to process. The second step is processed
	// many times in a loop.
	//
	// 1) Begin
	//    -----
	//    Split the first segment of path (called "path[1]" here)
	//    at parameter t and move newPath to this new knot. While
	//
	//        t = path[1]->param( m_radius )
	//
	//    as long as path[1] isnt too small (smaller than 2 * m_radius).
	//    In this case we set t = 0.5.
	//
	//    path:          newPath:
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
	//    Split the current segment path[n] at parameter t
	//    and add the first subsegment to newPath. While
	//
	//        t = path[n]->param( path[n]->length() - m_radius )
	//
	//    as long as path[n] isnt too small (smaller than 2 * m_radius).
	//    In this case we set t = 0.5.
	//
// TODO: round corner.
	//
	//    path:          newPath:
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
	//    path:          newPath:
	//
	//           3                     5
	//    X--X------X--X        6 .X------X. 4
	//    |            |         /          \
	//   4X            X2     7 X            X 3
	//    |            |      8 .\          /
	//    X--X------X--X        ...X------X. 2
	//           1                     1
	//

	double length;
	double param;

	// begin:
	if(
		path.isClosed() &&
		!(
			path.current()->type() == VSegment::curve &&
			path.getLast()->type() == VSegment::curve ) )
	{
		length = path.current()->length();

		param = length > 2 * m_radius
			? path.current()->param( m_radius )
			: param = 0.5;


		path.insert(
			path.current()->splitAt( param ) );

		newPath.moveTo(
			path.current()->knot() );

		path.next();
	}
	else
	{
		newPath.moveTo(
			path.current()->prev()->knot() );
	}


	// middle part:
	while(
		path.current() &&
		path.current()->next() )
	{
		if(
			!(
				path.current()->type() == VSegment::curve &&
				path.current()->next()->type() == VSegment::curve ) )
		{
			length = path.current()->length();

			param = length > 2 * m_radius
				? path.current()->param( length - m_radius )
				: 0.5;

			path.insert(
				path.current()->splitAt( param ) );
			newPath.append(
				path.current()->clone() );
			path.next();


			path.next();


			length = path.current()->length();

			param = length > 2 * m_radius
				? path.current()->param( m_radius )
				: 0.5;

			path.insert(
				path.current()->splitAt( param ) );


			// Round corner:
			newPath.curveTo(
				path.current()->prev()->point( 0.5 ),
				path.current()->point( 0.5 ),
				path.current()->knot() );
		}
		else
			newPath.append( path.current()->clone() );


		if( !success() )
			setSuccess();


		path.next();
	}


	// end:
	if( path.isClosed() )
	{
		if(
			!(
				path.current()->type() == VSegment::curve &&
				path.getFirst()->next()->type() == VSegment::curve ) )
		{
			length = path.current()->length();

			param = length > 2 * m_radius
				? path.current()->param( length - m_radius )
				: 0.5;

			path.insert(
				path.current()->splitAt( param ) );
			newPath.append(
				path.current()->clone() );
			path.next();


			path.first();
			path.next();

			// Round corner:
			newPath.curveTo(
				path.getLast()->point( 0.5 ),
				path.current()->point( 0.5 ),
				path.current()->knot() );
		}
		else
			newPath.append( path.current()->clone() );

		newPath.close();
	}
	else
		newPath.append( path.current()->clone() );


	path = newPath;

	// invalidate bounding box once:
	path.invalidateBoundingBox();
}
