/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>
#include <math.h>
#include <qwmatrix.h>

#include "vccmd_spiral.h"
#include "vglobal.h"
#include "vpath.h"

VCCmdSpiral::VCCmdSpiral( KarbonPart* part,
		double centerX, double centerY,
		double radius, uint segments, double fade, bool cw, double angle )
	: VCCommand( part, i18n( "Insert Spiral" ) ), m_centerX( centerX ), m_centerY( centerY ),
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
VCCmdSpiral::createPath()
{
	VPath* path = new VPath();
	path->fill().setFillRule( fillrule_winding );

	// advance by pi/2 clockwise or cclockwise?
	double adv_ang = ( m_clockWise ? -1.0 : 1.0 ) * VGlobal::pi_2;
	// radius of first segment is non-faded m_radius:
	double r = m_radius;
	double old_x = 0.0;
	double old_y = ( m_clockWise ? -1.0 : 1.0 ) * m_radius;
	double new_x;
	double new_y;
	double new_centerX = 0.0;
	double new_centerY = 0.0;
	path->moveTo( old_x, old_y );

	for ( uint i = 0; i < m_segments; ++i )
	{
		new_x = r * cos( adv_ang * ( i + 2 ) ) + new_centerX;
		new_y = r * sin( adv_ang * ( i + 2 ) ) + new_centerY;

		path->arcTo(
			old_x + new_x - new_centerX,
			old_y + new_y - new_centerY,
			new_x, new_y,
			r
		);

		new_centerX += ( new_x - new_centerX ) * ( 1.0 - m_fade );
		new_centerY += ( new_y - new_centerY ) * ( 1.0 - m_fade );
		old_x = new_x;
		old_y = new_y;
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
