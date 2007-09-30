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
#include <QString>
#define THUMB_SIZE 30

#include "vglobal.h"

VPattern::VPattern() : KoPattern("")
{
}

VPattern::VPattern( const QString &filename ) : KoPattern(filename)
{
	load();
	m_pixmap = QPixmap::fromImage(img(), Qt::AutoColor);
	if( img().width() > THUMB_SIZE || img().height() > THUMB_SIZE )
	{
		int xsize = THUMB_SIZE;
		int ysize = THUMB_SIZE;
		int picW  = img().width();
		int picH  = img().height();
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

		QImage thumbImg = img().scaled( xsize, ysize, Qt::IgnoreAspectRatio );
		setIcon(QIcon(QPixmap::fromImage( thumbImg )));
	}
}

#if 0
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
VPattern::load( const KoXmlElement& element )
{
	m_origin.setX( element.attribute( "originX", "0.0" ).toDouble() );
	m_origin.setY( element.attribute( "originY", "0.0" ).toDouble() );
	m_vector.setX( element.attribute( "vectorX", "0.0" ).toDouble() );
	m_vector.setY( element.attribute( "vectorY", "0.0" ).toDouble() );

	m_tilename = element.attribute( "tilename" );
	load( m_tilename );
}
#endif

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


