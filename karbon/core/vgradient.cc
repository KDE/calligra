/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vgradient.h"

VGradient::VGradient( VGradientType type )
	: m_type( type ), m_angle( 0.0 ), m_length( 0.0 )
{
}

void
VGradient::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "GRADIENT" );
	element.appendChild( me );

}

void
VGradient::load( const QDomElement& /*element*/ )
{
}

