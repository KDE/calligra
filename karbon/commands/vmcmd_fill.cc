/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_fill.h"
#include "vfill.h"

VMCmdFill::VMCmdFill( KarbonPart* part, const VColor &color )
	: VCommand( part, i18n( "Fill Objects" ) ), m_color( color )
{
	m_objects = m_part->selection();
	m_part->deselectAllObjects();

	if( m_objects.count() == 1 )
		setName( i18n( "Fill Object" ) );
}

void
VMCmdFill::execute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->fill().setColor( m_color );
	}
}

void
VMCmdFill::unexecute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->fill().setColor( m_color );
	}
}

