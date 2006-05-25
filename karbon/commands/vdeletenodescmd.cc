/* This file is doc of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <klocale.h>

#include "vdeletenodescmd.h"
#include "vselection.h"
#include "vsegment.h"
#include "vpath.h"
#include "vdocument.h"

VDeleteNodeCmd::VDeleteNodeCmd( VDocument *doc )
	: VCommand( doc, i18n( "Delete Node" ) )
{
}

VDeleteNodeCmd::~VDeleteNodeCmd()
{
}

void
VDeleteNodeCmd::visitVSubpath( VSubpath& path )
{
	VSegment* segment = path.first();

	path.next(); // skip begin segment
	while( segment )
	{
		if( segment->state() != VSegment::deleted && segment->knotIsSelected() )
		{
			segment->setState( VSegment::deleted );
			m_segments.append( segment );
		}
		segment = segment->next();
	}
	if( m_segments.count() > 0 )
		path.invalidateBoundingBox();
}

void
VDeleteNodeCmd::execute()
{
	VObjectListIterator itr( document()->selection()->objects() );

	for( ; itr.current() ; ++itr )
		visit( *itr.current() );

	setSuccess( m_segments.count() > 0 );
}

void
VDeleteNodeCmd::unexecute()
{
	Q3PtrListIterator<VSegment> itr( m_segments );
	for( ; itr.current() ; ++itr )
		itr.current()->setState( VSegment::normal );
	setSuccess( false );
}

