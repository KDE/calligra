/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>

#include <klocale.h>

#include "vaffinemap.h"
#include "vccmd_polygon.h"
#include "vglobal.h"
#include "vpath.h"

VCCmdPolygon::VCCmdPolygon( KarbonPart* part,
		const double centerX, const double centerY,
		const double radius, const uint edges )
	: VCommand( part, i18n("Create polygon-shape") ), m_object( 0L ),
	  m_centerX( centerX ), m_centerY( centerY )
{
	// a polygon should have at least 3 edges:
	m_edges = edges < 3 ? 3 : edges;

	// make sure the radius is positive:
	m_radius = radius < 0.0 ? -radius : radius;
}

void
VCCmdPolygon::execute()
{
	if ( m_object )
		m_object->setDeleted( false );
	else
	{
		m_object = new VPath();
		// we start at 90 degrees:
		m_object->moveTo( 0.0, m_radius );
		for ( int i = 0; i < m_edges; ++i )
		{
			m_object->lineTo(
				m_radius * cos( VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1 ) ),
				m_radius * sin( VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1 ) ) );
		}
		m_object->close();

		// translate path to center:
		VAffineMap aff_map;
		aff_map.translate( m_centerX, m_centerY );
		m_object->transform( aff_map );

		// add path:
		m_part->layers().getLast()->objects().append( m_object );
	}
}

void
VCCmdPolygon::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}
