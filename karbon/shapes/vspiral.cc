/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include <math.h>

#include <qwmatrix.h>

#include "vfill.h"
#include "vglobal.h"
#include "vspiral.h"


VSpiral::VSpiral( VObject* parent,
		const KoPoint& center, double radius, uint segments, double fade,
		bool clockwise, double angle )
	: VPath( parent )
{
	setDrawCenterNode();

	// It makes sense to have at least one segment:
	if( segments < 1 )
		segments = 1;

	// Make sure the radius is positive:
	if( radius < 0.0 )
		radius = -radius;

	// Fall back, when fade is out of range:
	if( fade <= 0.0 || fade >= 1.0 )
		fade = 0.5;


	VFill fill;
	fill.setFillRule( VFill::winding );
	setFill( fill );

	// advance by pi/2 clockwise or cclockwise?
	double adv_ang = ( clockwise ? -1.0 : 1.0 ) * VGlobal::pi_2;
	// radius of first segment is non-faded radius:
	double r = radius;

	KoPoint oldP( 0.0, ( clockwise ? -1.0 : 1.0 ) * radius );
	KoPoint newP;
	KoPoint newCenter( 0.0, 0.0 );
	moveTo( oldP );

	for ( uint i = 0; i < segments; ++i )
	{
		newP.setX( r * cos( adv_ang * ( i + 2 ) ) + newCenter.x() );
		newP.setY( r * sin( adv_ang * ( i + 2 ) ) + newCenter.y() );

		arcTo( oldP + newP - newCenter, newP, r );

		newCenter += ( newP - newCenter ) * ( 1.0 - fade );
		oldP = newP;
		r *= fade;
	}

	// translate path to center:
	QWMatrix m;
	m.translate( center.x(), center.y() );

	// sadly it's not feasible to simply add angle while creation.
	m.rotate(
		( angle + ( clockwise ? VGlobal::pi : 0.0 ) ) * // make cw-spiral start at mouse-pointer
			VGlobal::one_pi_180 );	// one_pi_180 = 1/(pi/180) = 180/pi.

	transform( m );
}

