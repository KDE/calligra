/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vselection.h"
#include "vwhirlpinch.h"
#include "vwhirlpinchcmd.h"


VWhirlPinchCmd::VWhirlPinchCmd( VDocument* doc,
	double angle, double pinch, double radius )
		: VCommand( doc, i18n( "Whirl Pinch" ) )
{
	m_selection = m_doc->selection()->clone();

	m_angle = angle;
	m_pinch = pinch;
	m_radius = radius;
	m_center = m_selection->boundingBox().center();
}

VWhirlPinchCmd::~VWhirlPinchCmd()
{
	delete( m_selection );
}

void
VWhirlPinchCmd::execute()
{
	VWhirlPinch op( m_center, m_angle, m_pinch, m_radius );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VWhirlPinchCmd::unexecute()
{
}

