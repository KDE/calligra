/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vshape.h"

void
VShape::save( QDomElement& element ) const
{
	m_stroke.save( element );
	m_fill.save( element );
}

void
VShape::load( const QDomElement& element )
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

