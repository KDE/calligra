/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdom.h>

#include "vcolor.h"

VColor::VColor( VColorSpace colorSpace )
{
	m_colorSpace = colorSpace;
	m_opacity = 1.0;

	m_value[0] = 0.0;
	m_value[1] = 0.0;
	m_value[2] = 0.0;
	m_value[3] = 0.0;
}

VColor::VColor( const VColor& color )
{
	m_colorSpace = color.m_colorSpace;
	m_opacity = color.m_opacity;

	m_value[0] = color.m_value[0];
	m_value[1] = color.m_value[1];
	m_value[2] = color.m_value[2];
	m_value[3] = color.m_value[3];
}

VColor::VColor( const QColor& color )
{
	m_colorSpace = rgb;
	m_opacity = 1.0;

	m_value[0] = color.red();
	m_value[1] = color.green();
	m_value[2] = color.blue();
}

QColor
VColor::toQColor() const
{
	VColor copy( *this );
	copy.convertToColorSpace( rgb );

	QColor color;
	color.setRgb( 255 * copy[0], 255 * copy[1], 255 * copy[2] );

	return color;
}

void
VColor::setColorSpace( const VColorSpace colorSpace, bool convert )
{
	if( convert )
		convertToColorSpace( colorSpace );

	m_colorSpace = colorSpace;
}

void
VColor::convertToColorSpace( const VColorSpace colorSpace )
{
	if( colorSpace == rgb )
	{
		if( m_colorSpace == rgb )
		{
			// Do nothing.
		}
		else if( m_colorSpace == cmyk )
		{
			m_value[0] = 1.0 - m_value[0] - m_value[3];
			m_value[1] = 1.0 - m_value[1] - m_value[3];
			m_value[2] = 1.0 - m_value[2] - m_value[3];
		}
		else if( m_colorSpace == hsb )
		{
			if( 1.0 + m_value[1] == 1.0 )	// saturation == 0.0
			{
				m_value[0] = m_value[2];	// brightness
				m_value[1] = m_value[2];
			}
			else
			{
// TODO
			}
		}
		else if( m_colorSpace == gray )
		{
			m_value[1] = m_value[0];
			m_value[2] = m_value[0];
		}
	}
	else if( colorSpace == cmyk )
	{
		if( m_colorSpace == rgb )
		{
			m_value[0] = 1.0 - m_value[0];
			m_value[1] = 1.0 - m_value[1];
			m_value[2] = 1.0 - m_value[2];
			m_value[3] = 0.0;
// TODO: undercolor removal
		}
		else if( m_colorSpace == cmyk )
		{
			// Do nothing.
		}
		else if( m_colorSpace == hsb )
		{
// TODO
		}
		else if( m_colorSpace == gray )
		{
			m_value[1] = 0.0;
			m_value[2] = 0.0;
			m_value[3] = 1.0 - m_value[0];
			m_value[0] = 0.0;
		}
	}
	else if( colorSpace == hsb )
	{
		if( m_colorSpace == rgb )
		{
// TODO
		}
		else if( m_colorSpace == cmyk )
		{
// TODO
		}
		else if( m_colorSpace == hsb )
		{
			// Do nothing.
		}
		else if( m_colorSpace == gray )
		{
			m_value[1] = 0.0;
			m_value[2] = m_value[0];
			m_value[0] = 0.0;
		}
	}
	else if( colorSpace == gray )
	{
		if( m_colorSpace == rgb )
		{
			m_value[0] =
				0.3  * m_value[0] +
				0.59 * m_value[1] +
				0.11 * m_value[2];
		}
		else if( m_colorSpace == cmyk )
		{
			m_value[0] =
				1.0 - QMIN( 1.0,
					0.3  * m_value[0] +
					0.59 * m_value[1] +
					0.11 * m_value[2] +
					m_value[3] );
		}
		else if( m_colorSpace == hsb )
		{
			m_value[0] = m_value[2];
		}
		else if( m_colorSpace == gray )
		{
			// Do nothing.
		}
	}
}

void
VColor::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "COLOR" );
	element.appendChild( me );

	me.setAttribute( "colorSpace", m_colorSpace );
	me.setAttribute( "opacity", m_opacity );

	if( m_colorSpace == gray )
		me.setAttribute( "v", m_value[0] );
	else
	{
		me.setAttribute( "v1", m_value[0] );
		me.setAttribute( "v2", m_value[1] );
		me.setAttribute( "v3", m_value[2] );

		if( m_colorSpace == cmyk )
			me.setAttribute( "v4", m_value[3] );
	}
}

void
VColor::load( const QDomElement& element )
{
	switch( element.attribute( "colorSpace" ).toUShort() )
	{
		case 1:
			m_colorSpace = cmyk; break;
		case 2:
			m_colorSpace = hsb; break;
		case 3:
			m_colorSpace = gray; break;
		default:
			m_colorSpace = rgb;
	}

	m_opacity = element.attribute( "opacity", "1.0" ).toFloat();

	if( m_colorSpace == gray )
		m_value[0] = element.attribute( "v", "0.0" ).toFloat();
	else
	{
		m_value[0] = element.attribute( "v1", "0.0" ).toFloat();
		m_value[1] = element.attribute( "v2", "0.0" ).toFloat();
		m_value[2] = element.attribute( "v3", "0.0" ).toFloat();

		if( m_colorSpace == cmyk )
			m_value[3] = element.attribute( "v4", "0.0" ).toFloat();
	}

	if( m_value[0] < 0.0 || m_value[0] > 1.0 )
		m_value[0] = 0.0;
	if( m_value[1] < 0.0 || m_value[1] > 1.0 )
		m_value[1] = 0.0;
	if( m_value[2] < 0.0 || m_value[2] > 1.0 )
		m_value[2] = 0.0;
	if( m_value[3] < 0.0 || m_value[3] > 1.0 )
		m_value[3] = 0.0;
}

