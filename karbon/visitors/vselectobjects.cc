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


#include "vselectobjects.h"
#include "vlayer.h"
#include "vdocument.h"
#include "vsegment.h"
#include <kdebug.h>

void
VSelectObjects::visitVComposite( VComposite& composite )
{
	if( !m_rect.isEmpty() )
	{
		if( m_select )
		{
			if( m_rect.intersects( composite.boundingBox() ) )
			{
				// extra intersection checks
				VPathListIterator itr( composite.paths() );

				for( itr.toFirst(); itr.current(); ++itr )
				{
					VPathIterator jtr( *( itr.current() ) );
					bool found = false;
					KoPoint p1, p2;
					KoPoint r1 = m_rect.topLeft(), r2 = m_rect.topRight(), r3 = m_rect.bottomRight(), r4 = m_rect.bottomLeft();
					while( jtr.current() && !found )
					{
						p1 = jtr.current()->point( 0 );
						if( jtr.current()->next() )
						{
							p2 = jtr.current()->next()->point( 0 );
							found |= jtr.current()->linesIntersect( r1, r2, p1, p2 );
							found |= jtr.current()->linesIntersect( r2, r3, p1, p2 );
							found |= jtr.current()->linesIntersect( r3, r4, p1, p2 );
							found |= jtr.current()->linesIntersect( r4, r1, p1, p2 );
						}
						++jtr;
					}
					// One more chance, if bbox totally contained in selection
					if( !found )
						found = m_rect.contains( composite.boundingBox(), true );

					if( found )
					{
						composite.setState( VObject::edit );
						m_selection.append( &composite );
						setSuccess();
						return;
					}
				}
			}
		}
		else
		{
			if( m_rect.intersects( composite.boundingBox() ) )
			{
				composite.setState( VObject::normal );
				m_selection.clear();
				setSuccess();
			}
		}
	}
	else
		visitVObject( composite );
}

void
VSelectObjects::visitVObject( VObject& object )
{
	// Never select a deleted object
	if( object.state() == VObject::deleted )
		return;

	if( !m_rect.isEmpty() )
	{
		if( m_select )
		{
			if( m_rect.intersects( object.boundingBox() ) )
			{
				object.setState( VObject::selected );
				m_selection.append( &object );
				setSuccess();
			}
		}
		else
		{
			if( m_rect.intersects( object.boundingBox() ) )
			{
				object.setState( VObject::normal );
				m_selection.clear();
				setSuccess();
			}
		}
	}
	else
	{
		if( m_select )
		{
			object.setState( VObject::selected );
			m_selection.append( &object );
			setSuccess();
		}
		else
		{
			object.setState( VObject::normal );
			setSuccess();
		}
	}
}

void
VSelectObjects::visitVLayer( VLayer& layer )
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
