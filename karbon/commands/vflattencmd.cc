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

#include "vflattencmd.h"
#include "vgroup.h"
#include "vpath.h"
#include "vsegment.h"
#include "vselection.h"


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
		: VCommand( doc, i18n( "Flatten" ) )
{
	m_selection = document()->selection()->clone();

	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

VFlattenCmd::~VFlattenCmd()
{
	delete( m_selection );
}

void
VFlattenCmd::execute()
{
	VObject* newObject;

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
/*		if( VGroup* group = dynamic_cast<VGroup*>( itr.current()->parent() ) )
		{
			newObject = itr.current()->clone();

			group->insertNewDeleteOld( newObject, itr.current() );
		}
		else
		{
*/
			newObject = itr.current();
//		}

		visit( *newObject );
	}
}

void
VFlattenCmd::unexecute()
{
}

void
VFlattenCmd::visitVPath( VPath& path )
{
	path.first();

	// ommit "begin" segment:
	while( path.next() )
	{
		while( !path.current()->isFlat( m_flatness )  )
		{
			// split at midpoint:
			path.insert(
				path.current()->splitAt( 0.5 ) );
		}

		// convert to line:
		path.current()->setType( VSegment::line );

		if( !success() )
			setSuccess();
	}
}
