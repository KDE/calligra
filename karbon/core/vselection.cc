/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vselection.h"

#include <kdebug.h>

VSelection::VSelection()
{
}

VSelection::~VSelection()
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
	{
		delete( itr.current() );
	}
}

void
VSelection::draw( VPainter *painter, const KoRect& rect )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect );
}

