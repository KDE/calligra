/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vobject.h"

void
VObject::save( QDomElement& element ) const
{
	m_stroke.save( element );
	m_fill.save( element );
}

void
VObject::load( const QDomElement& element )
{
	if( element.tagName() == "STROKE" )
	{
		m_stroke.load( element );
	}
	else if( element.tagName() == "FILL" )
	{
		m_fill.load( element );
	}
}

