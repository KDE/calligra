/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vccmd_rectangle.h"
#include "vpath.h"

#include <kdebug.h>
VCCmdRectangle::VCCmdRectangle( KarbonPart* part,
		const double tl_x, const double tl_y,
		const double br_x, const double br_y )
	: VCommand( part, i18n("Create rectangle-shape") ), m_object( 0L ),
	  m_tlX( tl_x ), m_tlY( tl_y ), m_brX( br_x ), m_brY( br_y )
{
}

void
VCCmdRectangle::execute()
{
	if ( m_object )
		m_object->setDeleted( false );
	else
	{
		m_object = new VPath();
		m_object->moveTo( m_tlX, m_tlY );
		m_object->lineTo( m_brX, m_tlY );
		m_object->lineTo( m_brX, m_brY );
		m_object->lineTo( m_tlX, m_brY );
		m_object->close();

		// add path:
		m_part->layers().getLast()->objects().append( m_object );
	}
}

void
VCCmdRectangle::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}
