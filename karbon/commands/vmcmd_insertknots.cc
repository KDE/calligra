/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_insertknots.h"
#include "vpath.h"
#include "vinsertknots.h"

VMCmdInsertKnots::VMCmdInsertKnots( KarbonPart* part, const VObjectList& objects,
	uint knots )
		: VCommand( part, i18n( "Insert Knots" ) ), m_objects( objects )
{
	m_knots = knots > 0 ? knots : 1;
}

void
VMCmdInsertKnots::execute()
{
	VInsertKnots op( m_knots );

	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VMCmdInsertKnots::unexecute()
{
}

