/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_group.h"
#include "vgroup.h"

VMCmdGroup::VMCmdGroup( KarbonPart* part )
	: VCommand( part, i18n( "Group Objects" ) )
{
	m_objects = m_part->selection();
	m_group = 0L;
}

void
VMCmdGroup::execute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		m_part->activeLayer()->removeRef( itr.current() );
	}
	m_group = new VGroup( m_objects );
	m_part->insertObject( m_group );
	m_part->selectObject( *m_group, true );
}

void
VMCmdGroup::unexecute()
{
	m_part->deselectAllObjects();
	VObjectListIterator itr( m_group->objects() );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		m_part->insertObject( itr.current() );
		m_part->selectObject( *( itr.current() ) );
	}
	// TODO : remove from corresponding VLayer
	m_part->activeLayer()->removeRef( m_group );
	m_group->empty();
	delete m_group;
	m_group = 0L;
}

