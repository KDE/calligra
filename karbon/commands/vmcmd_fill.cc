/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_fill.h"
#include "vfill.h"

VMCmdFill::VMCmdFill( KarbonPart* part, const VColor &color, float opacity )
	: VCommand( part, i18n( "Fill Objects" ) ), m_color( color ), m_opacity( opacity )
{
	m_objects = m_part->selection();
	//m_part->deselectAllObjects();

	if( m_objects.count() == 1 )
		setName( i18n( "Fill Object" ) );
}

void
VMCmdFill::execute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
	{
		//if( m_opacity == -1 )
		//	m_color.setOpacity( itr.current()->fill().color().opacity() );

		m_oldcolors.push_back( itr.current()->fill().color() );
		itr.current()->fill().setColor( m_color );
	}
}

void
VMCmdFill::unexecute()
{
	VObjectListIterator itr( m_objects );
	int i = 0;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->fill().setColor( m_oldcolors[ i++ ] );
	}
}

