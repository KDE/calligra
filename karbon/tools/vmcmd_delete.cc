/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_delete.h"

VMCmdDelete::VMCmdDelete( KarbonPart* part )
	: VCommand( part, i18n( "Delete Object(s)" ) )
{
	m_objects = m_part->selection();
	m_part->unselectAllObjects();
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

