/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include <math.h>

#include <qwmatrix.h>

#include "vglobal.h"
#include "vstar.h"


VStar::VStar( VObject* parent,
		const KoPoint& center, double outerRadius, double innerRadius,
		uint edges, double angle )
	: VPath( parent )
{
	// A star should have at least 3 edges:
	if( edges < 3 )
		edges = 3;

	// Make sure, radii are positive:
	if( outerRadius < 0.0 )
		outerRadius = -outerRadius;

	if( innerRadius < 0.0 )
		innerRadius = -innerRadius;


	// We start at angle + VGlobal::pi_2:
	KoPoint p(
		outerRadius * cos( angle + VGlobal::pi_2 ),
		outerRadius * sin( angle + VGlobal::pi_2 ) );
	moveTo( p );

	for ( uint i = 0; i < edges; ++i )
	{
		p.setX( innerRadius *
			cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) );
		p.setY( innerRadius *
			sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) );
		lineTo( p );

		p.setX( outerRadius *
			cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) );
		p.setY( outerRadius *
			sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) );
		lineTo( p );
	}
	close();

	// translate path to center:
	QWMatrix m;
	m.translate( center.x(), center.y() );
	transform( m );
}

