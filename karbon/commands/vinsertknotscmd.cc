/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vinsertknots.h"
#include "vinsertknotscmd.h"
#include "vselection.h"


VInsertKnotsCmd::VInsertKnotsCmd( VDocument *doc, uint knots )
		: VCommand( doc, i18n( "Insert Knots" ) )
{
	m_selection = m_doc->selection()->clone();

	m_knots = knots > 0 ? knots : 1;
}

VInsertKnotsCmd::~VInsertKnotsCmd()
{
	delete( m_selection );
}

void
VInsertKnotsCmd::execute()
{
	VInsertKnots op( m_knots );

	VObjectListIterator itr( m_selection->objects() );

	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VInsertKnotsCmd::unexecute()
{
}

