/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vccmd_rectangle.h"
#include "vpath.h"

VCCmdRectangle::VCCmdRectangle( KarbonPart* part,
		const double tlX, const double tlY,
		const double brX, const double brY, const double edgeR )
	: VCommand( part, i18n("Insert Rectangle") ), m_object( 0L )
{
	m_edgeR = edgeR < 0.0 ? 0.0 : edgeR;

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

	// catch case, when radius is larger than width or height:
	double minimum;
	if ( m_edgeR  > ( minimum = QMIN( ( m_brX - m_tlX ), ( m_tlY - m_brY ) ) * 0.5 ) )
		m_edgeR = minimum;
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
