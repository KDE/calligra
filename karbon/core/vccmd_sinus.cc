/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>

#include <klocale.h>

#include "vaffinemap.h"
#include "vccmd_sinus.h"
#include "vpath.h"

VCCmdSinus::VCCmdSinus( KarbonPart* part,
		const double tlX, const double tlY,
		const double brX, const double brY, const uint periods )
	: VCommand( part, i18n("Create Sinus") ), m_object( 0L ),
	  m_tlX( tlX ), m_tlY( tlY ), m_brX( brX ), m_brY( brY )
{
	// we want at least 1 period:
	m_periods = periods < 1 ? 1 : periods;
}

void
VCCmdSinus::execute()
{
	if ( m_object )
		m_object->setDeleted( false );
	else
	{
		m_object = new VPath();
		m_object->moveTo( 0.0, 0.0 );
		for ( uint i = 0; i < m_periods; ++i )
		{
			// i think 1/7 and sqrt( 2 ) etc. gets optimized away with -O2, no?
			m_object->curveTo(
				i + 1.0/24.0,	( 2.0 * sqrt( 2.0 ) - 1.0 ) / 7.0,
				i + 1.0/12.0,	( 4.0 * sqrt( 2.0 ) - 2.0 ) / 7.0,
				i + 1.0/8.0,	sqrt( 2.0 ) / 2.0 );
			m_object->curveTo(
				i + 1.0/6.0,	( 3.0 * sqrt( 2.0 ) + 2.0 ) / 7.0,
				i + 5.0/24.0,	1.0,
				i + 1.0/4.0,	1.0  );
			m_object->curveTo(
				i + 7.0/24.0,	1.0,
				i + 1.0/3.0,	( 3.0 * sqrt( 2.0 ) + 2.0 ) / 7.0,
				i + 3.0/8.0,	sqrt( 2.0 ) / 2.0 );
			m_object->curveTo(
				i + 5.0/12.0,	( 4.0 * sqrt( 2.0 ) - 2.0 ) / 7.0,
				i + 11.0/24.0,	( 2.0 * sqrt( 2.0 ) - 1.0 ) / 7.0,
				i + 1.0/2.0,	0.0 );
			m_object->curveTo(
				i + 13.0/24.0,	-( 2.0 * sqrt( 2.0 ) - 1.0 ) / 7.0,
				i + 7.0/12.0,	-( 4.0 * sqrt( 2.0 ) - 2.0 ) / 7.0,
				i + 5.0/8.0,	-sqrt( 2.0 ) / 2.0 );
			m_object->curveTo(
				i + 2.0/3.0,	-( 3.0 * sqrt( 2.0 ) + 2.0 ) / 7.0,
				i + 17.0/24.0,	-1.0,
				i + 3.0/4.0,	-1.0 );
			m_object->curveTo(
				i + 19.0/24.0,	-1.0,
				i + 5.0/6.0,	-( 3.0 * sqrt( 2.0 ) + 2.0 ) / 7.0,
				i + 7.0/8.0,	-sqrt( 2.0 ) / 2.0 );
			m_object->curveTo(
				i + 11.0/12.0,	-( 4.0 * sqrt( 2.0 ) - 2.0 ) / 7.0,
				i + 23.0/24.0,	-( 2.0 * sqrt( 2.0 ) - 1.0 ) / 7.0,
				i + 1.0,		0.0 );
		}

		double w = m_brX - m_tlX;
		double h = m_tlY - m_brY;

		// translate path and scale:
		VAffineMap aff_map;
		aff_map.scale( w/m_periods, h/2.0 );
		aff_map.translate( m_tlX, m_brY + h/2.0 );
		m_object->transform( aff_map );

		// add path:
		m_part->layers().getLast()->objects().append( m_object );
	}
}

void
VCCmdSinus::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}
