/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vfill.h"
#include "vfillcmd.h"


VFillCmd::VFillCmd( VDocument *doc, const VColor& color, float opacity )
	: VCommand( doc, i18n( "Fill Objects" ) ), m_color( color ), m_opacity( opacity )
{
	m_objects = m_doc->selection();
	//m_part->deselectAllObjects();

	if( m_objects.objects().count() == 1 )
		setName( i18n( "Fill Object" ) );
}

void
VFillCmd::execute()
{
	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
	{
		//if( m_opacity == -1 )
		//	m_color.setOpacity( itr.current()->fill().color().opacity() );

		m_oldcolors.push_back( VFill( *itr.current()->fill() ) );

		VFill fill = *itr.current()->fill();
		fill.setColor( m_color );

		itr.current()->setFill( fill );
	}
}

void
VFillCmd::unexecute()
{
	VObjectListIterator itr( m_objects.objects() );
	int i = 0;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setFill( m_oldcolors[ i++ ] );
	}
}

