/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

		if( m_offset != 0.0 )
			me.setAttribute( "offset", m_offset );

		QDomElement dash;

		Q3ValueListConstIterator<float> itr;
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
	for( int i = 0; i < list.count(); ++i )
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

