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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "vselectobjects.h"
#include "vlayer.h"
#include "vdocument.h"
#include "vsegment.h"
#include <kdebug.h>

void
VSelectObjects::visitVPath( VPath& composite )
{
	// Never select a deleted, locked or hidden object.
	if( composite.state() > VObject::normal &&
		composite.state() < VObject::selected )
		return;

	if( m_rectMode && m_rect.isEmpty() ) // in this mode everything is selected
	{
		visitVObject( composite );
		return;
	}

	bool selected = false;

	if( m_rectMode )
	{
		// Check if composite is completely inside the selection rectangle.
		// This test should be the first test since it's the less expensive one.
		if( m_rect.contains( composite.boundingBox() ) )
		{
			selected = true;
		}
	
		// Check if any of the rectangle corners is inside the composite.
		// This test should be done before the intersect test since it covers many
		// intersection cases.
		if( !selected )
		{
			if(
				composite.pointIsInside( m_rect.topLeft() ) ||
				composite.pointIsInside( m_rect.topRight() ) ||
				composite.pointIsInside( m_rect.bottomRight() ) ||
				composite.pointIsInside( m_rect.bottomLeft() ) )
			{
				selected = true;
			}
		}
	
		// Check if selection rectangle intersects the composite.
		if( !selected )
		{
			// Path for holding a helper segment.
			VSubpath path( 0L );
	
			path.moveTo( m_rect.topLeft() );
			path.lineTo( m_rect.topRight() );
	
			if( composite.intersects( *path.getLast() ) )
			{
				selected = true;
			}
			else
			{
				path.getFirst()->setKnot( m_rect.bottomRight() );
	
				if( composite.intersects( *path.getLast() ) )
				{
					selected = true;
				}
				else
				{
					path.getLast()->setKnot( m_rect.bottomLeft() );
	
					if( composite.intersects( *path.getLast() ) )
					{
						selected = true;
					}
					else
					{
						path.getFirst()->setKnot( m_rect.topLeft() );
	
						if( composite.intersects( *path.getLast() ) )
						{
							selected = true;
						}
					}
				}
			}
		}
	}
	else
	{
		if( composite.pointIsInside( m_point ) )
			selected = true;
	}

	if( selected )
	{
		if( m_select )
		{
			composite.setState( VObject::selected );
			if( ! m_selection.containsRef( &composite ) )
				m_selection.append( &composite );
		}
		else
		{
			composite.setState( VObject::normal );
			m_selection.remove( &composite );
		}

		setSuccess();
	}
}

void
VSelectObjects::visitVObject( VObject& object )
{
	// Never select a deleted, locked or hidden object.
	if( object.state() > VObject::normal &&
		object.state() < VObject::selected )
		return;

	// selection by selection rectangle
	if( m_rectMode )
	{
		if( !m_rect.isEmpty() )
		{
			if( m_select )
			{
				if( m_rect.intersects( object.boundingBox() ) )
				{
					object.setState( VObject::selected );
					if( ! m_selection.containsRef( &object ) )
						m_selection.append( &object );
					setSuccess();
				}
			}
			else
			{
				if( m_rect.intersects( object.boundingBox() ) )
				{
					object.setState( VObject::normal );
					m_selection.remove( &object );
					setSuccess();
				}
			}
		}
		else
		{
			if( m_select )
			{
				object.setState( VObject::selected );
				if( ! m_selection.containsRef( &object ) )
					m_selection.append( &object );
				setSuccess();
			}
			else
			{
				object.setState( VObject::normal );
				m_selection.remove( &object );
				setSuccess();
			}
		}
	}
	// selection by point
	else
	{
		if( object.boundingBox().contains( m_point ) )
		{
			if( m_select )
			{
				object.setState( VObject::selected );
				if( ! m_selection.containsRef( &object ) )
					m_selection.append( &object );
			}
			else
			{
				object.setState( VObject::normal );
				m_selection.remove( &object );
			}
			setSuccess();
		}
	}

}

void
VSelectObjects::visitVLayer( VLayer& layer )
{
	/* TODO: porting to flake 
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
	*/
}

void 
VSelectObjects::visitVText( VText& text )
{
	// Never select a deleted, locked or hidden object.
	if( text.state() > VObject::normal &&
		text.state() < VObject::selected )
		return;

	int deselectedGlyphs = 0;

	VPathListIterator itr( text.glyphs() );
	for( ; itr.current(); ++itr )
	{
		VPath c( 0L );
		c.combine( *itr.current() );
		visitVPath( c );
		if( m_select && c.state() == VObject::selected )
		{
			kDebug(38000) << "selected: " << itr.current() << endl;
			m_selection.remove( &c );
			text.setState( VObject::selected );
			if( ! m_selection.containsRef( &text ) )
				m_selection.append( &text );
			return;
		}
		else if( c.state() == VObject::normal )
		{
			kDebug(38000) << "deselected: " << itr.current() << endl;
			deselectedGlyphs++;
		}
	}
	if( deselectedGlyphs >= 0 && uint( deselectedGlyphs ) == text.glyphs().count() )
	{
		text.setState( VObject::normal );
		m_selection.remove( &text );
	}
}

void 
VSelectObjects::visitVGroup( VGroup& group )
{ 
	// Never select a deleted, locked or hidden object.
	if( group.state() > VObject::normal &&
		group.state() < VObject::selected )
		return;

	if( ! m_insideGroups )
		visitVObject( group );
	else
	{
		VVisitor::visitVGroup( group );
	}
}
