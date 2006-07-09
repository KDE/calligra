/* This file is part of the KDE project
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

#include "vgroup.h"
#include "vgroupcmd.h"
#include "vselection.h"
#include "vdocument.h"
#include "vlayer.h"

VGroupCmd::VGroupCmd( VDocument *doc )
	: VCommand( doc, i18n( "Group Objects" ), "14_group" )
{
	m_selection = document()->selection()->clone();

	m_group = 0L;
}

VGroupCmd::~VGroupCmd()
{
	delete( m_selection );
}

void
VGroupCmd::execute()
{
	m_group = new VGroup( document()->activeLayer() );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		// remove from corresponding parent
		VGroup *parent = dynamic_cast<VGroup*>( itr.current()->parent() );
		if( parent )
			parent->take( *itr.current() );

		m_group->append( itr.current() );
	}

	document()->append( m_group );
	document()->selection()->clear();
	document()->selection()->append( m_group );

	setSuccess( true );
}

void
VGroupCmd::unexecute()
{
	if( ! m_group ) 
		return;

	document()->selection()->clear();

	VObjectListIterator itr( m_group->objects() );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		document()->selection()->append( itr.current() );
	}

	VGroup* parent;
	if( ( parent = dynamic_cast<VGroup*>( m_group->parent() ) ) )
	{
		// unregister from parent:
		parent->take( *m_group );

		// inform all objects in this group about their new parent
		VObjectListIterator itr = m_selection->objects();

		for ( ; itr.current() ; ++itr )
		{
			parent->append( itr.current() );
		}

		m_group->clear();
		m_group->setState( VObject::deleted );
	}
	
	setSuccess( false );
}

