/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>

#include <klocale.h>

#include "vaffinemap.h"
#include "vccmd_star.h"
#include "vglobal.h"
#include "vpath.h"

VCCmdStar::VCCmdStar( KarbonPart* part,
		const double centerX, const double centerY,
		const double outerR, const double innerR, const uint edges )
	: VCommand( part, i18n("Insert Star") ), m_object( 0L ),
	  m_centerX( centerX ), m_centerY( centerY )
{
	// a star should have at least 3 edges:
	m_edges = edges < 3 ? 3 : edges;

	// make sure, radii are positive:
	m_outerR = outerR < 0.0 ? -outerR : outerR;
	m_innerR = innerR < 0.0 ? -innerR : innerR;
}

void
VCCmdStar::execute()
{
	if ( m_object )
		m_object->setDeleted( false );
	else
	{
		m_object = createPath();

		// add path:
		m_part->insertObject( m_object );
	}
}

void
VCCmdStar::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}

VPath*
VCCmdStar::createPath()
{
	VPath* path = new VPath();

	// we start at 90 degrees:
	path->moveTo( 0.0, m_outerR );
	for ( int i = 0; i < m_edges; ++i )
	{
		path->lineTo(
			m_innerR * cos( VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 ) ),
			m_innerR * sin( VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 ) ) );
		path->lineTo(
			m_outerR * cos( VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 ) ),
			m_outerR * sin( VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 ) ) );
	}
	path->close();

	// translate path to center:
	VAffineMap aff_map;
	aff_map.translate( m_centerX, m_centerY );
	path->transform( aff_map );

	return path;
}
