/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vdeletecmd.h"


VDeleteCmd::VDeleteCmd( VDocument *doc )
	: VCommand( doc, i18n( "Delete Objects" ) )
{
	m_objects = m_doc->selection();
	m_doc->deselect();

	if( m_objects.objects().count() == 1 )
		setName( i18n( "Delete Object" ) );
}

void
VDeleteCmd::execute()
{
	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( state_deleted );
	}
}

void
VDeleteCmd::unexecute()
{
	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( state_normal );
	}
}

