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


VRoundCornersCmd::VRoundCornersCmd( VDocument* doc, double radius )
	: VReplacingCmd( doc, i18n( "Round Corners" ) )
{
	// Set members.
	m_radius = radius > 0.0 ? radius : 1.0;
}

void
VRoundCornersCmd::visitVPath( VPath& path )
{
	// Optimize and avoid a crash.
	if( path.isEmpty() )
		return;

	// Note: we modiy segments from path. that doesnt hurt, since we
	// replace "path" with the temporary path "newPath" afterwards.

	VPath newPath( 0L );

	path.first();
	// Skip "begin".
	path.next();

	// This algorithm is worked out by <kudling@kde.org> to produce similar results as
	// the "round corners" algorithms found in other applications. Neither code nor
	// algorithms from any 3rd party is used though.
	//
	// We want to replace all corners with round corners having "radius" m_radius.
	// The algorithm doesn't really produce circular arcs, but that's ok since
	// the algorithm achieves nice looking results and is generic enough to be applied
	// to all kind of paths.
	// Note also, that this algorithm doesn't touch curve/curve joins, since they
	// are usually smooth enough.
	//
	// We'll manipulate the input path for bookkeeping purposes and construct a new
	// temporary path in parallel. We finally replace the input path with the new path.
	//
	//
	// Without restricting generality, let's assume the input path is closed and
	// contains segments which build a rectangle.
	//
	//           2
	//    O------------O
	//    |            |        Numbers reflect the segments' order
	//   3|            |1       in the path. We neglect the "begin"
	//    |            |        segment here.
	//    O------------O
	//           0
	//
	// There are three unique steps to process. The second step is processed
	// many times in a loop.
	//
	// 1) Begin
	//    -----
	//    Split the first segment of the input path (called "path[0]" here)
	//    at parameter t
	//
	//        t = path[0]->param( m_radius )
	//
	//    and move newPath to this new knot. If current segment is too small
	//    (smaller than 2 * m_radius), we always set t = 0.5 here and in the further
	//    steps as well.
	//
	//    path:                 new path:
	//
	//           2
	//    O------------O
	//    |            |
	//  3 |            | 1                    The current segment is marked with "#"s.
	//    |            |
	//    O##O#########O        ...O
	//           0                     0
	//
	// 2) Loop
	//    ----
	//    The loop step is iterated over all segments. After each appliance the index n
	//    is incremented and the loop step is reapplied until no untouched segment is left.
	//
	//    Split the current segment path[n] of the input path at parameter t
	//
	//        t = path[n]->param( path[n]->length() - m_radius )
	//
	//    and add the first subsegment of the curent segment to newPath.
	//
	//    path:                 new path:
	//
	//           2
	//    O------------O
	//    |            |
	//  3 |            | 1
	//    |            |
	//    O--O######O##O           O------O...
	//           0                     0
	//
	//    Now make the second next segment (the original path[1] segment in our example)
	//    the current one. Split it at paramter t
	//
	//        t = path[n]->param( m_radius )
	//
	//    path:                 new path:
	//
	//           2
	//    O------------O
	//    |            #
	//  3 |            O 1
	//    |            #
	//    O--O------O--O           O------O...
	//           0                     0
	//
	//    Make the first subsegment of the current segment the current one.
	//
	//    path:                 new path:
	//
	//           2
	//    O------------O
	//    |            |
	//  3 |            O 1                   O
	//    |            #                    /.1
	//    O--O------O--O           O------O...
	//           0                     0
	//
	// 3) End
	//    ---
	//
	//    path:                 new path:
	//
	//           2                     4
	//    O--O------O--O        5 .O------O. 3
	//    |            |         /          \
	//  3 O            O 1    6 O            O 2
	//    |            |      7 .\          /
	//    O--O------O--O        ...O------O. 1
	//           0                     0
	//

	double length;
	double param;

	// "Begin" step.
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


		if( !success() )
			setSuccess();
	}
	else
	{
		newPath.moveTo(
			path.current()->prev()->knot() );
	}


	// "Middle" step.
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


			// Round corner.
			newPath.curveTo(
				path.current()->prev()->point( 0.5 ),
				path.current()->point( 0.5 ),
				path.current()->knot() );


			if( !success() )
				setSuccess();
		}
		else
			newPath.append( path.current()->clone() );

		path.next();
	}


	// "End" step.
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

			// Round corner.
			newPath.curveTo(
				path.getLast()->point( 0.5 ),
				path.current()->point( 0.5 ),
				path.current()->knot() );


			if( !success() )
				setSuccess();
		}
		else
			newPath.append( path.current()->clone() );

		newPath.close();
	}
	else
		newPath.append( path.current()->clone() );


	path = newPath;

	// Invalidate bounding box once.
	path.invalidateBoundingBox();
}

