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


#include <qwmatrix.h>

#include "vglobal.h"
#include "vsinus.h"


VSinus::VSinus( VObject* parent,
		const KoPoint& topLeft, double width, double height, uint periods )
	: VPath( parent )
{
	// We want at least 1 period:
	if( periods < 1 )
		periods = 1;

	KoPoint p1;
	KoPoint p2;
	KoPoint p3( 0.0, 0.0 );
	moveTo( p3 );

	for ( uint i = 0; i < periods; ++i )
	{
		p1.setX( i + 1.0/24.0 );
		p1.setY( ( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p2.setX( i + 1.0/12.0 );
		p2.setY( ( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0/8.0 );
		p3.setY( VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 1.0/6.0 );
		p1.setY( ( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 5.0/24.0 );
		p2.setY( 1.0 );
		p3.setX( i + 1.0/4.0 );
		p3.setY( 1.0 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 7.0/24.0 );
		p1.setY( 1.0 );
		p2.setX( i + 1.0/3.0 );
		p2.setY( ( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 3.0/8.0 );
		p3.setY( VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 5.0/12.0 );
		p1.setY( ( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 11.0/24.0 );
		p2.setY( ( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0/2.0 );
		p3.setY( 0.0 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 13.0/24.0 );
		p1.setY( -( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p2.setX( i + 7.0/12.0 );
		p2.setY( -( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 5.0/8.0 );
		p3.setY( -VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 2.0/3.0 );
		p1.setY( -( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 17.0/24.0 );
		p2.setY( -1.0 );
		p3.setX( i + 3.0/4.0 );
		p3.setY( -1.0 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 19.0/24.0 );
		p1.setY( -1.0 );
		p2.setX( i + 5.0/6.0 );
		p2.setY( -( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 7.0/8.0 );
		p3.setY( -VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 11.0/12.0 );
		p1.setY( -( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 23.0/24.0 );
		p2.setY( -( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0 );
		p3.setY( 0.0 );
		curveTo( p1, p2, p3 );
	}

	// Translate and scale:
	QWMatrix m;
	m.translate( topLeft.x(), topLeft.y() - height * 0.5 );
	m.scale( width / periods, height * 0.5 );
	transform( m );
}

