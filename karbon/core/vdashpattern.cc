/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vdashpattern.h"


VDashPattern::VDashPattern( double offset )
	: m_offset( offset )
{
}

void
VDashPattern::save( QDomElement& element ) const
{
	if( m_array.size() != 0 )
	{
		QDomElement me = element.ownerDocument().createElement( "DASHPATTERN" );
		element.appendChild( me );

		me.setAttribute( "offset", m_offset );

		QDomElement dash;

		QValueListConstIterator<float> itr;
		for( itr = m_array.begin(); itr != m_array.end(); ++itr )
		{
			dash = element.ownerDocument().createElement( "DASH" );
			me.appendChild( dash );
			dash.setAttribute( "l", *( itr ) );
		}
	}
}

void
VDashPattern::load( const QDomElement& element )
{
	m_offset = element.attribute( "offset", "0.0" ).toDouble();

	float value;

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "DASH" )
			{
				value = e.attribute( "l", "0.0" ).toFloat();
				if( value < 0.0 )
					value = 0.0;

				m_array.append( value );
			}
		}
	}
}

