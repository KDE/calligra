/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vflatten.h"
#include "vflattencmd.h"
#include "vselection.h"


VFlattenCmd::VFlattenCmd( VDocument *doc, double flatness )
		: VCommand( doc, i18n( "Flatten" ) )
{
	m_selection = m_doc->selection()->clone();

	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

VFlattenCmd::~VFlattenCmd()
{
	delete( m_selection );
}

void
VFlattenCmd::execute()
{
	VFlatten op( m_flatness );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VFlattenCmd::unexecute()
{
}

