/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include <math.h>

#include <qwmatrix.h>

#include "vglobal.h"
#include "vpolygon.h"


VPolygon::VPolygon( VObject* parent,
		const KoPoint& center, double radius, uint edges, double angle )
	: VPath( parent )
{
	// A polygon should have at least 3 edges:
	if( edges < 3 )
		edges = 3;

	// Make sure the radius is positive:
	if( radius < 0.0 )
		radius = -radius;


	// we start at m_angle + VGlobal::pi_2:
	KoPoint p(
		radius * cos( angle + VGlobal::pi_2 ),
		radius * sin( angle + VGlobal::pi_2 ) );

	moveTo( p );

	for ( uint i = 0; i < edges - 1; ++i )
	{
		p.setX(
			radius * cos( angle + VGlobal::pi_2
			+ VGlobal::twopi / edges * ( i + 1 ) ) );
		p.setY(
			radius * sin( angle + VGlobal::pi_2
			+ VGlobal::twopi / edges * ( i + 1 ) ) );

		lineTo( p );
	}
	close();

	// translate path to center:
	QWMatrix m;
	m.translate( center.x(), center.y() );
	transform( m );
}

