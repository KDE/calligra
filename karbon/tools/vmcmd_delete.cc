/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_delete.h"
#include "vpath.h"

VMCmdDelete::VMCmdDelete( KarbonPart* part )
	: VCommand( part, i18n("Delete Object") )
{
}

void
VMCmdDelete::execute()
{
	m_part->deleteObjects( m_objects );
}

void
VMCmdDelete::unexecute()
{
	QPtrListIterator<VObject> itr = m_objects;
	for ( ; itr.current(); ++itr )
	{
		itr.current()->setState( VObject::selected );
	}
}

