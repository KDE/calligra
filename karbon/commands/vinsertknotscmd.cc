/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vinsertknots.h"
#include "vinsertknotscmd.h"


VInsertKnotsCmd::VInsertKnotsCmd( VDocument *doc, uint knots )
		: VCommand( doc, i18n( "Insert Knots" ) )
{
	m_knots = knots > 0 ? knots : 1;
	m_objects = doc->selection();
}

void
VInsertKnotsCmd::execute()
{
	VInsertKnots op( m_knots );

	VObjectListIterator itr( m_objects.objects() );

	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VInsertKnotsCmd::unexecute()
{
}

