/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qwmatrix.h>

#include <klocale.h>

#include "vglobal.h"
#include "vpath.h"
#include "vstarcmd.h"


VStarCmd::VStarCmd( VDocument *doc,
		double centerX, double centerY,
		double outerR, double innerR, uint edges, double angle )
	: VShapeCmd( doc, i18n( "Insert Star" ) ), m_centerX( centerX ), m_centerY( centerY ),
		m_angle( angle )
{
	// a star should have at least 3 edges:
	m_edges = edges < 3 ? 3 : edges;

	// make sure, radii are positive:
	m_outerR = outerR < 0.0 ? -outerR : outerR;
	m_innerR = innerR < 0.0 ? -innerR : innerR;
}

VObject*
VStarCmd::createPath()
{
	VPath* path = new VPath( 0L );

	// we start at m_angle + VGlobal::pi_2:
	KoPoint p(
		m_outerR * cos( m_angle + VGlobal::pi_2 ),
		m_outerR * sin( m_angle + VGlobal::pi_2 ) );
	path->moveTo( p );

	for ( uint i = 0; i < m_edges; ++i )
	{
		p.setX( m_innerR *
			cos( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 ) ) );
		p.setY( m_innerR *
			sin( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 ) ) );
		path->lineTo( p );

		p.setX( m_outerR *
			cos( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 ) ) );
		p.setY( m_outerR *
			sin( m_angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 ) ) );
		path->lineTo( p );
	}
	path->close();

	// translate path to center:
	QWMatrix m;
	m.translate( m_centerX, m_centerY );
	path->transform( m );

	return path;
}

