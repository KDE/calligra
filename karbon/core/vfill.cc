/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vfill.h"

VFill::VFill()
	: m_opacity( 1.0 ), m_mode( pserver_none ), m_fillRule( evenOdd )
{
}

void
VFill::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "FILL" );
	element.appendChild( me );

	// save color:
	m_color.save( me );

	// 
	me.setAttribute( "fillRule", m_fillRule );
}

void
VFill::load( const QDomElement& element )
{
	m_fillRule = element.attribute( "fillRule" ) == 0 ? evenOdd : winding;

	// load color:
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "COLOR" )
			{
				m_color.load( e );
			}
		}
	}
}



