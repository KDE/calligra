/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vobjectlist.h"

#include <qrect.h>

#include <kdebug.h>

VObjectList::VObjectList()
{
}

VObjectList::~VObjectList()
{
}

QRect
VObjectList::boundingBox( const double zoomFactor ) const
{
	if(count() > 0)
	{
		QRect rect = getFirst()->boundingBox( zoomFactor );
	    VObjectListIterator itr( *this );
		++itr;
	    for ( ; itr.current() ; ++itr )
	        rect = rect.unite( itr.current()->boundingBox( zoomFactor ) );

		return rect;
	}
	else
		return QRect();
}

