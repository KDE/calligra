/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>

#include <kdebug.h>
#include <klocale.h>

#include "vaffinemap.h"
#include "vccmd_polygon.h"
#include "vglobal.h"
#include "vpath.h"

VCCmdPolygon::VCCmdPolygon( KarbonPart* part, const double center_x, const double center_y,
		const double radius, const uint edges )
	: VCommand( part, i18n("Create polygon-shape") ), m_object( 0L ),
	  m_centerX( center_x ), m_centerY( center_y )
{
	// a polygon should have at least 3 edges:
	m_edges = edges < 3 ? 3 : edges;

	// make sure the radius is positive:
	m_radius = radius < 0 ? -radius : radius;
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
		m_object->moveTo( 0, m_radius );
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
		m_part->layers().last()->objects().append( m_object );
	}
}

void
VCCmdPolygon::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}
