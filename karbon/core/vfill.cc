/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vfill.h"

VFill::VFill()
	: m_opacity( 1.0 )
{
}

void
VFill::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "FILL" );
	element.appendChild( me );


}

void
VFill::load( const QDomElement& /*element*/ )
{

}



