/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vgroup.h"
#include "vgroupcmd.h"


VGroupCmd::VGroupCmd( VDocument *doc )
	: VCommand( doc, i18n( "Group Objects" ) )
{
	m_objects = m_doc->selection();
	m_group = 0L;
}

void
VGroupCmd::execute()
{
	m_group = new VGroup( m_doc->activeLayer() );

	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		m_doc->activeLayer()->take( itr.current() );
		m_group->append( itr.current() );
	}

	m_doc->append( m_group );
	m_doc->select( *m_group, true );
}

void
VGroupCmd::unexecute()
{
	m_doc->deselect();

	VObjectListIterator itr( m_group->objects() );
	for ( ; itr.current() ; ++itr )
	{
		// TODO : remove from corresponding VLayer
		m_doc->select( *itr.current() );
	}

	VGroup* parent;
	if( ( parent = dynamic_cast<VGroup*>( m_group->parent() ) ) )
	{
		// unregister from parent:
		parent->take( m_group );

		// inform all objects in this group about their new parent
		VObjectListIterator itr = m_objects.objects();

		for ( ; itr.current() ; ++itr )
			parent->append( itr.current() );

		m_group->clear();
	}
	
	delete m_group;
	m_group = 0L;
}
