/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vdeletecmd.h"
#include "vselection.h"


VDeleteCmd::VDeleteCmd( VDocument* doc )
	: VCommand( doc, i18n( "Delete Objects" ) )
{
	m_selection = m_doc->selection()->clone();

	m_doc->selection()->clear();

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Delete Object" ) );
}

VDeleteCmd::~VDeleteCmd()
{
	delete( m_selection );
}

void
VDeleteCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( VObject::deleted );
	}
}

void
VDeleteCmd::unexecute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( VObject::normal );
	}
}

