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

#include <klocale.h>

#include "vzordercmd.h"
#include "vselection.h"
#include "vdocument.h"
#include "vlayer.h"

VZOrderCmd::VZOrderCmd( VDocument *doc, VOrder state )
	: VCommand( doc, i18n( "Order Selection" ) ), m_state( state )
{
	m_selection = document()->selection()->clone();
}

VZOrderCmd::VZOrderCmd( VDocument *doc, VObject *obj, VOrder state )
	: VCommand( doc, i18n( "Order Selection" ) ), m_state( state )
{
	m_selection = new VSelection();
	m_selection->append( obj );
}

VZOrderCmd::~VZOrderCmd()
{
	delete( m_selection );
}

void
VZOrderCmd::execute()
{
	if( m_state == sendToBack )
	{
		VObjectListIterator itr( document()->selection()->objects() );
		for ( itr.toLast() ; itr.current() ; --itr )
		{
			// remove from old layer
			VObjectList objects;
			VLayerListIterator litr( document()->layers() );

			for ( ; litr.current(); ++litr )
			{
				objects = litr.current()->objects();
				VObjectListIterator itr2( objects );
				for ( ; itr2.current(); ++itr2 )
					if( itr2.current() == itr.current() )
					{
						litr.current()->sendToBack( *itr2.current() );
						itr2.current()->setState( VObject::selected );
					}
			}
		}
	}
	else if( m_state == bringToFront )
	{
		VObjectListIterator itr( document()->selection()->objects() );
		for ( ; itr.current() ; ++itr )
		{
			// remove from old layer
			VObjectList objects;
			VLayerListIterator litr( document()->layers() );

			for ( ; litr.current(); ++litr )
			{
				objects = litr.current()->objects();
				VObjectListIterator itr2( objects );
				for ( ; itr2.current(); ++itr2 )
					if( itr2.current() == itr.current() )
					{
						litr.current()->bringToFront( *itr2.current() );
						itr2.current()->setState( VObject::selected );
					}
			}
		}
	}
	else if( m_state == up || m_state == down )
	{
		VSelection selection = *m_selection;
		// TODO : this doesn't work for objects inside groups!
		VLayerListIterator litr( document()->layers() );
		while( !selection.objects().isEmpty() && litr.current() )
		{
			for ( ; litr.current(); ++litr )
			{
				if( litr.current()->state() == VObject::deleted )
					continue;
				VObjectList objects = litr.current()->objects();
				VObjectList todo;
				VObjectListIterator objectItr( objects );
				// find all selected VObjects that are in the current layer
				for ( ; objectItr.current(); ++objectItr )
				{
					VObjectListIterator selectionItr( selection.objects() );
					for ( ; selectionItr.current() ; ++selectionItr )
					{
						if( objectItr.current() == selectionItr.current() )
						{
							if( m_state == up )
								todo.prepend( objectItr.current() );
							else
								todo.append( objectItr.current() );
						}
					}
				}

				kdDebug(38000) << "todo.count() : " << todo.count() << endl;

				// we have found the affected vobjects in this vlayer
				VObjectListIterator todoItr( todo );
				for ( ; todoItr.current(); ++todoItr )
				{
					if( m_state == up )
						litr.current()->upwards( *todoItr.current() );
					else
						litr.current()->downwards( *todoItr.current() );
					// remove from selection
					selection.take( *todoItr.current() );
					// make sure object stays selected
					todoItr.current()->setState( VObject::selected );
				}
			}
		}
	}
	setSuccess( true );
}

void
VZOrderCmd::unexecute()
{
	if( m_state == sendToBack )
	{
		m_state = bringToFront;
		execute();
		m_state = sendToBack;
	}
	else if( m_state == bringToFront )
	{
		m_state = sendToBack;
		execute();
		m_state = bringToFront;
	}
	else if( m_state == up )
	{
		m_state = down;
		execute();
		m_state = up;
	}
	else if( m_state == down )
	{
		m_state = up;
		execute();
		m_state = down;
	}
	setSuccess( false );
}

