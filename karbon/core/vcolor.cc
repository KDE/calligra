/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vcolor.h"

VColor::VColor()
	: m_colorSpace( rgb )
{
	m_value[0] = 0.0;
	m_value[1] = 0.0;
	m_value[2] = 0.0;
	m_value[3] = 0.0;
}

void
VColor::pseudoValues( int& v1, int& v2, int& v3 ) const
{
	if( m_colorSpace == rgb )
	{
		v1 = qRound( 255 * m_value[0] );
		v2 = qRound( 255 * m_value[1] );
		v3 = qRound( 255 * m_value[2] );
	}
	else
	{
		double copy[3];

		convertToColorSpace( rgb, &copy[0], &copy[1], &copy[2] );

		v1 = qRound( 255 * copy[0] );
		v2 = qRound( 255 * copy[1] );
		v3 = qRound( 255 * copy[2] );
	}
}

void
VColor::values(
	double* v1, double* v2,
	double* v3, double* v4 ) const
{
	if( v1 )
		*v1 = m_value[0];
	if( v2 )
		*v2 = m_value[1];
	if( v3 )
		*v3 = m_value[2];
	if( v4 )
		*v4 = m_value[3];
}

void
VColor::setColorSpace( const VColorSpace colorSpace )
{
	if( colorSpace == m_colorSpace )
		return;

	convertToColorSpace( colorSpace,
		&m_value[0], &m_value[1], &m_value[2], &m_value[3] )
;
	m_colorSpace = colorSpace;
}

bool
VColor::convertToColorSpace( const VColorSpace colorSpace,
	double* v1, double* v2, double* v3, double* v4 ) const
{
	double copy[4];

	if( colorSpace == rgb )
	{
		if( m_colorSpace == rgb )
		{
			copy[0] = m_value[0];
			copy[1] = m_value[1];
			copy[2] = m_value[2];
		}
		else if( m_colorSpace == cmyk )
		{
			copy[0] = 1.0 - m_value[0] - m_value[3];
			copy[1] = 1.0 - m_value[1] - m_value[3];
			copy[2] = 1.0 - m_value[2] - m_value[3];
		}
		else if( m_colorSpace == hsb )
		{
			if( 1.0 + m_value[1] == 1.0 )	// saturation == 0.0
			{
				copy[0] = m_value[2];	// brightness
				copy[1] = m_value[2];
				copy[2] = m_value[2];
			}
			else
			{
// TODO
			}
		}
		else if( m_colorSpace == gray )
		{
			copy[0] = m_value[0];
			copy[1] = m_value[0];
			copy[2] = m_value[0];
		}
	}
	else if( colorSpace == cmyk )
	{
		if( m_colorSpace == rgb )
		{
			copy[0] = 1.0 - m_value[0];
			copy[1] = 1.0 - m_value[1];
			copy[2] = 1.0 - m_value[2];
			copy[3] = 0.0;
// TODO: undercolor removal
		}
		else if( m_colorSpace == cmyk )
		{
			copy[0] = m_value[0];
			copy[1] = m_value[1];
			copy[2] = m_value[2];
			copy[3] = m_value[3];
		}
		else if( m_colorSpace == hsb )
		{
// TODO
		}
		else if( m_colorSpace == gray )
		{
			copy[0] = 0.0;
			copy[1] = 0.0;
			copy[2] = 0.0;
			copy[3] = 1.0 - m_value[0];
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
			copy[0] = m_value[0];
			copy[1] = m_value[1];
			copy[2] = m_value[2];
		}
		else if( m_colorSpace == gray )
		{
			copy[0] = 0.0;
			copy[1] = 0.0;
			copy[2] = m_value[0];
		}
	}
	else if( colorSpace == gray )
	{
		if( m_colorSpace == rgb )
		{
			copy[0] =
				0.3  * m_value[0] +
				0.59 * m_value[1] +
				0.11 * m_value[2];
		}
		else if( m_colorSpace == cmyk )
		{
			copy[0] =
				1.0 - QMIN( 1.0,
					0.3  * m_value[0] +
					0.59 * m_value[1] +
					0.11 * m_value[2] +
					m_value[3] );
		}
		else if( m_colorSpace == hsb )
		{
			copy[0] = m_value[2];
		}
		else if( m_colorSpace == gray )
		{
			copy[0] = m_value[0];
		}
	}

	if( v1 )
		*v1 = copy[0];
	if( v2 )
		*v2 = copy[1];
	if( v3 )
		*v3 = copy[2];
	if( v4 )
		*v4 = copy[3];
}

