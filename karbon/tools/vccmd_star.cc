/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>
#include <math.h>
#include <qwmatrix.h>

#include "vccmd_star.h"
#include "vglobal.h"
#include "vpath.h"

VCCmdStar::VCCmdStar( KarbonPart* part,
		double centerX, double centerY,
		double outerR, double innerR, uint edges, double angle )
	: VCCommand( part, i18n( "Insert Star" ) ), m_centerX( centerX ), m_centerY( centerY ),
		m_angle( angle )
{
	// a star should have at least 3 edges:
	m_edges = edges < 3 ? 3 : edges;

	// make sure, radii are positive:
	m_outerR = outerR < 0.0 ? -outerR : outerR;
	m_innerR = innerR < 0.0 ? -innerR : innerR;
}

VPath*
VCCmdStar::createPath()
{
	VPath* path = new VPath();

	// we start at m_angle + VGlobal::pi_2:
	path->moveTo(
		m_outerR * cos( m_angle + VGlobal::pi_2 ),
		m_outerR * sin( m_angle + VGlobal::pi_2 ) );

	for ( uint i = 0; i < m_edges; ++i )
	{
		path->lineTo(
			m_innerR * cos( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 ) ),
			m_innerR * sin( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 ) ) );
		path->lineTo(
			m_outerR * cos( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 ) ),
			m_outerR * sin( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 ) ) );
	}
	path->close();

	// translate path to center:
	QWMatrix m;
	m.translate( m_centerX, m_centerY );
	path->transform( m );

	return path;
}
