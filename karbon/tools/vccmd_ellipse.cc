/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vaffinemap.h"
#include "vccmd_ellipse.h"
#include "vpath.h"

#include <kdebug.h>
VCCmdEllipse::VCCmdEllipse( KarbonPart* part,
		const double tlX, const double tlY,
		const double brX, const double brY )
	: VCommand( part, i18n("Insert Ellipse") ), m_object( 0L )
{
	// make sure that tl is really top-left and br is bottom-right:
	if ( tlX < brX )
	{
		m_tlX = tlX;
		m_brX = brX;
	}
	else
	{
		m_tlX = brX;
		m_brX = tlX;
	}
	if ( tlY > brY )
	{
		m_tlY = tlY;
		m_brY = brY;
	}
	else
	{
		m_tlY = brY;
		m_brY = tlY;
	}
}

void
VCCmdEllipse::execute()
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
VCCmdEllipse::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}

VPath*
VCCmdEllipse::createPath()
{
	// first create unity-circle around origin (0,0):
	VPath* path = new VPath();
	path->moveTo( -0.5, 0.0 );
	path->arcTo( -0.5,  0.5,  0.0,  0.5, 0.5 );
	path->arcTo(  0.5,  0.5,  0.5,  0.0, 0.5 );
	path->arcTo(  0.5, -0.5,  0.0, -0.5, 0.5 );
	path->arcTo( -0.5, -0.5, -0.5,  0.0, 0.5 );
	path->close();

	double w = m_brX - m_tlX;
	double h = m_tlY - m_brY;

	// translate path and scale:
	VAffineMap aff_map;
	aff_map.scale( w, h );
	aff_map.translate( m_tlX + w*0.5, m_brY + h*0.5 );
	path->transform( aff_map );

	return path;
}
