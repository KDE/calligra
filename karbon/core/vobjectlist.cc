/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vobjectlist.h"

#include <koRect.h>

#include <kdebug.h>

VObjectList::VObjectList()
	: m_boundingBoxIsInvalid( true )
{
}

VObjectList::~VObjectList()
{
}

const KoRect&
VObjectList::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		VObjectListIterator itr( *this );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			m_boundingBox |= itr.current()->boundingBox();
		}

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

