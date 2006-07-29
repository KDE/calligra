/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "vflattencmd.h"
#include <klocale.h>

#include <core/vpath.h>
#include <core/vsegment.h>

// TODO: Think about if we want to adapt this:

/*
 * <cite from GNU ghostscript's gxpflat.c>
 *
 * To calculate how many points to sample along a path in order to
 * approximate it to the desired degree of flatness, we define
 *      dist((x,y)) = abs(x) + abs(y);
 * then the number of points we need is
 *      N = 1 + sqrt(3/4 * D / flatness),
 * where
 *      D = max(dist(p0 - 2*p1 + p2), dist(p1 - 2*p2 + p3)).
 * Since we are going to use a power of 2 for the number of intervals,
 * we can avoid the square root by letting
 *      N = 1 + 2^(ceiling(log2(3/4 * D / flatness) / 2)).
 * (Reference: DEC Paris Research Laboratory report #1, May 1989.)
 *
 * We treat two cases specially.  First, if the curve is very
 * short, we halve the flatness, to avoid turning short shallow curves
 * into short straight lines.  Second, if the curve forms part of a
 * character (indicated by flatness = 0), we let
 *      N = 1 + 2 * max(abs(x3-x0), abs(y3-y0)).
 * This is probably too conservative, but it produces good results.
 *
 * </cite from GNU ghostscript's gxpflat.c>
 */


VFlattenCmd::VFlattenCmd( VDocument *doc, double flatness )
	: VReplacingCmd( doc, i18n( "Flatten Curves" ) )
{
	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

void
VFlattenCmd::visitVSubpath( VSubpath& path )
{
	path.first();

	// Ommit first segment.
	while( path.next() )
	{
		while( !path.current()->isFlat( m_flatness )  )
		{
			// Split at midpoint.
			path.insert(
				path.current()->splitAt( 0.5 ) );
		}

		// Convert to line.
		path.current()->setDegree( 1 );

		if( !success() )
			setSuccess();
	}
}

