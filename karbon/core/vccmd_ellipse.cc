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
	: VCommand( part, i18n("Create ellipse-shape") ), m_object( 0L ),
	  m_tlX( tlX ), m_tlY( tlY ), m_brX( brX ), m_brY( brY )
{
}

void
VCCmdEllipse::execute()
{
	if ( m_object )
		m_object->setDeleted( false );
	else
	{
		// first create unity-circle around origin (0,0):
		m_object = new VPath();
		m_object->moveTo( -0.5, 0.0 );
		m_object->arcTo( -0.5,  0.5,  0.0,  0.5, 0.5 );
		m_object->arcTo(  0.5,  0.5,  0.5,  0.0, 0.5 );
		m_object->arcTo(  0.5, -0.5,  0.0, -0.5, 0.5 );
		m_object->arcTo( -0.5, -0.5, -0.5,  0.0, 0.5 );
		m_object->close();

		double w = m_brX - m_tlX;
		double h = m_tlY - m_brY;

		// translate path and scale:
		VAffineMap aff_map;
		aff_map.scale( w, h );
		aff_map.translate( m_tlX + w/2, m_brY + h/2 );
		m_object->transform( aff_map );

		// add path:
		m_part->layers().getLast()->objects().append( m_object );
	}
}

void
VCCmdEllipse::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}
