/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qwmatrix.h>

#include <klocale.h>

#include "vglobal.h"
#include "vpath.h"
#include "vpolygoncmd.h"


VPolygonCmd::VPolygonCmd( VDocument *doc,
		double centerX, double centerY,
		double radius, uint edges, double angle )
	: VShapeCmd( doc, i18n( "Insert Polygon" ) ), m_centerX( centerX ), m_centerY( centerY ),
		m_angle( angle )
{
	// a polygon should have at least 3 edges:
	m_edges = edges < 3 ? 3 : edges;

	// make sure the radius is positive:
	m_radius = radius < 0.0 ? -radius : radius;
}

VObject*
VPolygonCmd::createPath()
{
	VPath* path = new VPath();

	// we start at m_angle + VGlobal::pi_2:
	KoPoint p(
		m_radius * cos( m_angle + VGlobal::pi_2 ),
		m_radius * sin( m_angle + VGlobal::pi_2 ) );

	path->moveTo( p );

	for ( uint i = 0; i < m_edges; ++i )
	{
		p.setX(
			m_radius * cos( m_angle + VGlobal::pi_2
			+ VGlobal::twopi / m_edges * ( i + 1 ) ) );
		p.setY(
			m_radius * sin( m_angle + VGlobal::pi_2
			+ VGlobal::twopi / m_edges * ( i + 1 ) ) );

		path->lineTo( p );
	}
	path->close();

	// translate path to center:
	QWMatrix m;
	m.translate( m_centerX, m_centerY );
	path->transform( m );

	return path;
}

