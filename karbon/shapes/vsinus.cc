/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include <qwmatrix.h>

#include "vglobal.h"
#include "vsinus.h"


VSinus::VSinus( VObject* parent,
		const KoPoint& topLeft, const KoPoint& bottomRight, uint periods )
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

	double w = bottomRight.x() - topLeft.x();
	double h = topLeft.y() - bottomRight.y();

	// Translate and scale:
	QWMatrix m;
	m.translate( topLeft.x(), bottomRight.y() + h * 0.5 );
	m.scale( w / periods, h * 0.5 );
	transform( m );
}

