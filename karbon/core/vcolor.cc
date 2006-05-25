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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include <qdom.h>

#include "vcolor.h"
#include "vglobal.h"


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

	m_value[0] = color.red() / 255.0;
	m_value[1] = color.green() / 255.0;
	m_value[2] = color.blue() / 255.0;
}

VColor::operator QColor() const
{
	VColor copy( *this );
	copy.convertToColorSpace( rgb );

	QColor color;
	color.setRgb( int( 255 * copy[0] ), int( 255 * copy[1] ), int( 255 * copy[2] ) );

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
	// TODO: numerical stability.
	// TODO: undercolor removal with cmyk.

	if( colorSpace == rgb )
	{
		if( m_colorSpace == rgb )
		{
			// Do nothing.
		}
		else if( m_colorSpace == cmyk )
		{
			m_value[0] = 1.0 - qMin( 1.0f, m_value[0] + m_value[3] );
			m_value[1] = 1.0 - qMin( 1.0f, m_value[1] + m_value[3] );
			m_value[2] = 1.0 - qMin( 1.0f, m_value[2] + m_value[3] );
		}
		else if( m_colorSpace == hsb )
		{
			// Achromatic case (saturation == 0.0).
			if( m_value[1] == 0.0 )
			{
				// Set to brightness:
				m_value[0] = m_value[2];
				m_value[1] = m_value[2];
				m_value[2] = m_value[2];	// For readability.
			}
			else
			{
				float hue6 = 6.0 * m_value[0];
				uint i = static_cast<uint>( hue6 );
				float f = hue6 - i;

				float m = m_value[2] * ( 1.0 - m_value[1] );
				float n = m_value[2] * ( 1.0 - m_value[1] * f );
				float k = m_value[2] * ( 1.0 - m_value[1] * ( 1.0 - f ) );

				float r;
				float g;
				float b;

				switch( i )
				{
					case 1:
						r = n;
						g = m_value[2];
						b = m;
					break;
					case 2:
						r = m;
						g = m_value[2];
						b = k;
					break;
					case 3:
						r = m;
						g = n;
						b = m_value[2];
					break;
					case 4:
						r = k;
						g = m;
						b = m_value[2];
					break;
					case 5:
						r = m_value[2];
						g = m;
						b = n;
					break;
					default:
						r = m_value[2];
						g = k;
						b = m;
				}

				m_value[0] = r;
				m_value[1] = g;
				m_value[2] = b;
			}
		}
		else if( m_colorSpace == Qt::gray )
		{
			m_value[0] = m_value[0];	// For readability.
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
		}
		else if( m_colorSpace == cmyk )
		{
			// Do nothing.
		}
		else if( m_colorSpace == hsb )
		{
// TODO
		}
		else if( m_colorSpace == Qt::gray )
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
			if(
				m_value[0] == m_value[1] &&
				m_value[1] == m_value[2] )
			{
				// Arbitrary:
				m_value[3] = m_value[0];
				m_value[1] = 0.0;
				m_value[2] = 0.0;
			}
			else
			{
				float max;
				float min;

				// Find maximum + minimum rgb component:
				if( m_value[0] > m_value[1] )
				{
					max = m_value[0];
					min = m_value[1];
				}
				else
				{
					max = m_value[1];
					min = m_value[0];
				}

				if( m_value[2] > max )
					max = m_value[2];

				if( m_value[2] < min )
					min = m_value[2];


				float hue;
				const float diff = max - min;

				// Which rgb component is maximum?
				if( max == m_value[0] )
					// Red:
					hue = ( m_value[1] - m_value[2] ) * VGlobal::one_6 / diff;
				else if( max == m_value[1] )
					// Green:
					hue = ( m_value[2] - m_value[0] ) * VGlobal::one_6 / diff + VGlobal::one_3;
				else
					// Blue:
					hue = ( m_value[0] - m_value[1] ) * VGlobal::one_6 / diff + VGlobal::two_3;

				if( hue < 0.0 )
					hue += 1.0;


				m_value[0] = hue;
				m_value[1] = diff / max;
				m_value[2] = max;
			}
		}
		else if( m_colorSpace == cmyk )
		{
// TODO
		}
		else if( m_colorSpace == hsb )
		{
			// Do nothing.
		}
		else if( m_colorSpace == Qt::gray )
		{
			m_value[1] = 0.0;
			m_value[2] = m_value[0];
			m_value[0] = 0.0;
		}
	}
	else if( colorSpace == Qt::gray )
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
				1.0 - qMin( 1.0,
					0.3  * m_value[0] +
					0.59 * m_value[1] +
					0.11 * m_value[2] +
					m_value[3] );
		}
		else if( m_colorSpace == hsb )
		{
			m_value[0] = m_value[2];
		}
		else if( m_colorSpace == Qt::gray )
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

	if( m_colorSpace != rgb )
		me.setAttribute( "colorSpace", m_colorSpace );
	if( m_opacity != 1.0 )
		me.setAttribute( "opacity", m_opacity );

	if( m_colorSpace == Qt::gray )
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

	if( m_colorSpace == Qt::gray )
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

