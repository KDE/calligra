/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vccmd_rectangle.h"
#include "vpath.h"

#include <kdebug.h>
VCCmdRectangle::VCCmdRectangle( KarbonPart* part,
		const double tlX, const double tlY,
		const double brX, const double brY, const double edgeR )
	: VCommand( part, i18n("Create Rectangle") ), m_object( 0L ),
	  m_tlX( tlX ), m_tlY( tlY ), m_brX( brX ), m_brY( brY )
{
// TODO: catch case when radius is larger than height/width?
	m_edgeR = edgeR < 0.0 ? 0.0 : edgeR;
}

void
VCCmdRectangle::execute()
{
	if ( m_object )
		m_object->setDeleted( false );
	else
	{
		m_object = new VPath();

		// create non-rounded rectangle?
		if ( m_edgeR == 0.0 )
		{
			m_object->moveTo( m_tlX, m_tlY );
			m_object->lineTo( m_brX, m_tlY );
			m_object->lineTo( m_brX, m_brY );
			m_object->lineTo( m_tlX, m_brY );
		}
		else	// create rounded rectangle:
		{
			m_object->moveTo( m_tlX, m_tlY - m_edgeR );
			m_object->arcTo(
				m_tlX, m_tlY,
				m_tlX + m_edgeR, m_tlY, m_edgeR );
			m_object->arcTo(
				m_brX, m_tlY,
				m_brX, m_tlY - m_edgeR, m_edgeR );
			m_object->arcTo(
				m_brX, m_brY,
				m_brX - m_edgeR, m_brY, m_edgeR );
			m_object->arcTo(
				m_tlX, m_brY,
				m_tlX, m_brY + m_edgeR, m_edgeR );
		}
		m_object->close();

		// add path:
		m_part->insertObject( m_object );
	}
}

void
VCCmdRectangle::unexecute()
{
	if ( m_object )
		m_object->setDeleted();
}
