/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qwmatrix.h>

#include <klocale.h>

#include "vfill.h"
#include "vglobal.h"
#include "vpath.h"
#include "vspiralcmd.h"


VSpiralCmd::VSpiralCmd( VDocument *doc,
		double centerX, double centerY,
		double radius, uint segments, double fade, bool cw, double angle )
	: VShapeCmd( doc, i18n( "Insert Spiral" ) ), m_centerX( centerX ), m_centerY( centerY ),
		m_clockWise( cw ), m_angle( angle )
{
	// it makes sense to have at least one segment:
	m_segments = segments < 1 ? 1 : segments;

	// make sure the radius is positive:
	m_radius = radius < 0.0 ? -radius : radius;

	// fall back, when fade is out of range:
	m_fade = ( fade <= 0.0 || fade >= 1.0 ) ? 0.5 : fade;
}

VObject*
VSpiralCmd::createPath()
{
	VPath* path = new VPath( 0L );

	VFill fill;
	fill.setFillRule( fillrule_winding );
	path->setFill( fill );

	// advance by pi/2 clockwise or cclockwise?
	double adv_ang = ( m_clockWise ? -1.0 : 1.0 ) * VGlobal::pi_2;
	// radius of first segment is non-faded m_radius:
	double r = m_radius;

	KoPoint oldP( 0.0, ( m_clockWise ? -1.0 : 1.0 ) * m_radius );
	KoPoint newP;
	KoPoint newCenter( 0.0, 0.0 );
	path->moveTo( oldP );

	for ( uint i = 0; i < m_segments; ++i )
	{
		newP.setX( r * cos( adv_ang * ( i + 2 ) ) + newCenter.x() );
		newP.setY( r * sin( adv_ang * ( i + 2 ) ) + newCenter.y() );

		path->arcTo( oldP + newP - newCenter, newP, r );

		newCenter += ( newP - newCenter ) * ( 1.0 - m_fade );
		oldP = newP;
		r *= m_fade;
	}

	// translate path to center:
	QWMatrix m;
	m.translate( m_centerX, m_centerY );

	// sadly it's not feasible to simply add m_angle while creation.
	m.rotate(
		( m_angle + ( m_clockWise ? VGlobal::pi : 0.0 ) ) * // make cw-spiral start at mouse-pointer
			VGlobal::one_pi_180 );	// one_pi_180 = 1/(pi/180) = 180/pi.

	path->transform( m );

	return path;
}

