/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qwmatrix.h>

#include <klocale.h>

#include "vglobal.h"
#include "vpath.h"
#include "vsinuscmd.h"


VSinusCmd::VSinusCmd( VDocument *doc,
		const double tlX, const double tlY,
		const double brX, const double brY, const uint periods )
	: VShapeCmd( doc, i18n( "Insert Sinus" ) ),
	  m_tlX( tlX ), m_tlY( tlY ), m_brX( brX ), m_brY( brY )
{
	// we want at least 1 period:
	m_periods = periods < 1 ? 1 : periods;
}

VObject*
VSinusCmd::createPath()
{
	VPath* path = new VPath();

	KoPoint p1;
	KoPoint p2;
	KoPoint p3( 0.0, 0.0 );
	path->moveTo( p3 );

	for ( uint i = 0; i < m_periods; ++i )
	{
		p1.setX( i + 1.0/24.0 );
		p1.setY( ( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p2.setX( i + 1.0/12.0 );
		p2.setY( ( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0/8.0 );
		p3.setY( VGlobal::sqrt2 * 0.5 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 1.0/6.0 );
		p1.setY( ( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 5.0/24.0 );
		p2.setY( 1.0 );
		p3.setX( i + 1.0/4.0 );
		p3.setY( 1.0 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 7.0/24.0 );
		p1.setY( 1.0 );
		p2.setX( i + 1.0/3.0 );
		p2.setY( ( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 3.0/8.0 );
		p3.setY( VGlobal::sqrt2 * 0.5 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 5.0/12.0 );
		p1.setY( ( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 11.0/24.0 );
		p2.setY( ( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0/2.0 );
		p3.setY( 0.0 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 13.0/24.0 );
		p1.setY( -( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p2.setX( i + 7.0/12.0 );
		p2.setY( -( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 5.0/8.0 );
		p3.setY( -VGlobal::sqrt2 * 0.5 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 2.0/3.0 );
		p1.setY( -( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 17.0/24.0 );
		p2.setY( -1.0 );
		p3.setX( i + 3.0/4.0 );
		p3.setY( -1.0 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 19.0/24.0 );
		p1.setY( -1.0 );
		p2.setX( i + 5.0/6.0 );
		p2.setY( -( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 7.0/8.0 );
		p3.setY( -VGlobal::sqrt2 * 0.5 );
		path->curveTo( p1, p2, p3 );

		p1.setX( i + 11.0/12.0 );
		p1.setY( -( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 23.0/24.0 );
		p2.setY( -( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0 );
		p3.setY( 0.0 );
		path->curveTo( p1, p2, p3 );
	}

	double w = m_brX - m_tlX;
	double h = m_tlY - m_brY;

	// translate path and scale:
	QWMatrix m;
	m.translate( m_tlX, m_brY + h*0.5 );
	m.scale( w/m_periods, h*0.5 );
	path->transform( m );

	return path;
}

