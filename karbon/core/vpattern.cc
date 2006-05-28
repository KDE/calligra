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

#include "vpattern.h"
#include <qpixmap.h>
#define THUMB_SIZE 30

#include "vglobal.h"

VPattern::VPattern()
{
	m_valid = false;
	validThumb = false;
}

VPattern::VPattern( const QString &tilename )
{
	load( tilename );
}

void
VPattern::load( const QString &tilename )
{
	m_tilename = tilename;
	bool ok = m_image.load( tilename );

	if( !ok )
	{
		m_valid = false;
		return;
	}
 
	m_image = m_image.convertDepth( 32 );
	m_pixmap.convertFromImage(m_image, Qt::AutoColor);
	if( m_image.width() > THUMB_SIZE || m_image.height() > THUMB_SIZE )
	{
		int xsize = THUMB_SIZE;
		int ysize = THUMB_SIZE;
		int picW  = m_image.width();
		int picH  = m_image.height();
		if( picW > picH )
		{
			float yFactor = (float)((float)(float)picH/(float)picW);
			ysize = (int)(yFactor * (float)THUMB_SIZE);
			if(ysize > 30) ysize = 30;
		}
		else if( picW < picH )
		{
			float xFactor = (float)((float)picW/(float)picH);
			xsize = (int)(xFactor * (float)THUMB_SIZE);
			if(xsize > 30) xsize = 30;
		}

		QImage thumbImg = m_image.smoothScale( xsize, ysize );
		m_pixmapThumb.convertFromImage( thumbImg );
		validThumb = true;
	}
	m_valid = !m_image.isNull();
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

	me.setAttribute( "tilename", m_tilename );

	element.appendChild( me );
}

void
VPattern::load( const QDomElement& element )
{
	m_origin.setX( element.attribute( "originX", "0.0" ).toDouble() );
	m_origin.setY( element.attribute( "originY", "0.0" ).toDouble() );
	m_vector.setX( element.attribute( "vectorX", "0.0" ).toDouble() );
	m_vector.setY( element.attribute( "vectorY", "0.0" ).toDouble() );

	m_tilename = element.attribute( "tilename" );
	load( m_tilename );
}

void
VPattern::transform( const QMatrix &m )
{
	m_origin = VGlobal::transformPoint( m_origin, m );
	m_vector = VGlobal::transformPoint( m_vector, m );
}

QPixmap& VPattern::pixmap() const
{
	return (QPixmap&)m_pixmap;
}

QPixmap& VPattern::thumbPixmap() const
{
	return (QPixmap&)m_pixmapThumb;
}



