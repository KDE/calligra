/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vobjectlist.h"

#include <koRect.h>

#include <kdebug.h>

VObjectList::VObjectList()
{
}

VObjectList::~VObjectList()
{
}

KoRect
VObjectList::boundingBox( const double zoomFactor ) const
{
	if( count() > 0 )
	{
		KoRect rect = getFirst()->boundingBox( zoomFactor );
		VObjectListIterator itr( *this );
		++itr;
		for ( ; itr.current() ; ++itr )
			rect |= itr.current()->boundingBox( zoomFactor );

		return rect;
	}
	else
		return KoRect();
}

