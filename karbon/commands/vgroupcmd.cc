/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vgroup.h"
#include "vgroupcmd.h"


VGroupCmd::VGroupCmd( KarbonPart* part )
	: VCommand( part, i18n( "Group Objects" ) )
{
	m_objects = m_part->document().selection();
	m_group = 0L;
}

void
VGroupCmd::execute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		m_part->document().activeLayer()->removeRef( itr.current() );
	}
	m_group = new VGroup( m_objects );
	m_part->insertObject( m_group );
	m_part->document().selectObject( *m_group, true );
}

void
VGroupCmd::unexecute()
{
	m_part->document().deselectAllObjects();
	VObjectListIterator itr( m_group->objects() );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		m_part->insertObject( itr.current() );
		m_part->document().selectObject( *( itr.current() ) );
	}
	// TODO : remove from corresponding VLayer
	m_part->document().activeLayer()->removeRef( m_group );
	m_group->empty();
	delete m_group;
	m_group = 0L;
}

