/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vroundcorners.h"
#include "vroundcornerscmd.h"
#include "vselection.h"


VRoundCornersCmd::VRoundCornersCmd( VDocument* doc, double radius )
	: VCommand( doc, i18n( "Round Corners" ) )
{
	m_selection = m_doc->selection()->clone();

	m_radius = radius > 0.0 ? radius : 1.0;
}

VRoundCornersCmd::~VRoundCornersCmd()
{
	delete( m_selection );
}

void
VRoundCornersCmd::execute()
{
	VRoundCorners op( m_radius );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VRoundCornersCmd::unexecute()
{
}

