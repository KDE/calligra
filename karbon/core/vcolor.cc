/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
VColor::values( VColorSpace colorSpace,
	int* v1, int* v2, int* v3, int* v4 ) const
{
	if( colorSpace == rgb )
	{
		if( v1 )
			*v1 = qRound( 255 * m_value[0] );
		if( v2 )
			*v1 = qRound( 255 * m_value[1] );
		if( v3 )
			*v1 = qRound( 255 * m_value[2] );
	}
}

void
VColor::setColorSpace( VColorSpace colorSpace )
{
// TODO: convert values
	m_colorSpace = colorSpace;
}