/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_delete.h"

VMCmdDelete::VMCmdDelete( KarbonPart* part )
	: VCommand( part, i18n( "Delete Objects" ) )
{
	m_objects = m_part->selection();
	m_part->deselectAllObjects();

	if( m_objects.count() == 1 )
		setName( i18n( "Delete Object" ) );
}

void
VMCmdDelete::execute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( VObject::deleted );
	}
}

void
VMCmdDelete::unexecute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( VObject::normal );
	}
}

