/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vpattern.h"

VPattern::VPattern( const QString &tilename )
{
	m_image.load( tilename );
}

unsigned char *
VPattern::pixels()
{
	return m_image.bits();
}

unsigned int
VPattern::tileWidth() const
{
	return m_image.width();
}

unsigned int
VPattern::tileHeight() const
{
	return m_image.height();
}

void
VPattern::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "PATTERN" );

	me.setAttribute( "originX", m_origin.x() );
	me.setAttribute( "originY", m_origin.y() );
	me.setAttribute( "vectorX", m_vector.x() );
	me.setAttribute( "vectorY", m_vector.y() );

	element.appendChild( me );
}

void
VPattern::load( const QDomElement& element )
{
	m_origin.setX( element.attribute( "originX", "0.0" ).toDouble() );
	m_origin.setY( element.attribute( "originY", "0.0" ).toDouble() );
	m_vector.setX( element.attribute( "vectorX", "0.0" ).toDouble() );
	m_vector.setY( element.attribute( "vectorY", "0.0" ).toDouble() );
}

void
VPattern::transform( const QWMatrix &m )
{
	m_origin = m_origin.transform( m );	
	m_vector = m_vector.transform( m );	
}


