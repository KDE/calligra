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


#include "vpath.h"
#include "vsegment.h"
#include "vselectnodes.h"
#include "vlayer.h"
#include "vdocument.h"


void
VSelectNodes::visitVPath( VPath& path )
{
	path.first();

	// skip "begin":
	while( path.next() )
	{
		if( m_rect.isEmpty() )
		{
			path.current()->selectCtrlPoint1( m_select );
			path.current()->selectCtrlPoint2( m_select );
			path.current()->selectKnot( m_select );

			setSuccess();
		}
		else
		{
			if(
				path.current()->ctrlPointFixing() != VSegment::first &&
				m_rect.contains( path.current()->ctrlPoint1() ) )
			{
				// select first control point, when previous knot is selected:
				if(
					path.current()->prev() &&
					path.current()->prev()->knotSelected() )
				{
					path.current()->selectCtrlPoint1( m_select );

					setSuccess();
				}
			}

			if(
				path.current()->ctrlPointFixing() != VSegment::second &&
				m_rect.contains( path.current()->ctrlPoint2() ) )
			{
				// select second control point, when knot is selected:
				if( path.current()->knotSelected() )
				{
					path.current()->selectCtrlPoint2( m_select );

					setSuccess();
				}
			}

			if( m_rect.contains( path.current()->knot() ) )
			{
				path.current()->selectKnot( m_select );

				setSuccess();
			}
		}
	}
}

void
VSelectNodes::visitVLayer( VLayer& layer )
{
	VDocument* doc = (VDocument*)layer.parent();
	if ( ( layer.state() != VObject::deleted ) &&
	     ( ( doc->selectionMode() == VDocument::AllLayers ) ||
	       ( doc->selectionMode() == VDocument::VisibleLayers && ( layer.state() == VObject::normal || layer.state() == VObject::normal_locked ) ) ||
	       ( doc->selectionMode() == VDocument::SelectedLayers && layer.selected() ) ||
	       ( doc->selectionMode() == VDocument::ActiveLayer && doc->activeLayer() == &layer ) ) )
	{
		VObjectListIterator itr( layer.objects() );
		for( ; itr.current(); ++itr )
			itr.current()->accept( *this );
	}
}
