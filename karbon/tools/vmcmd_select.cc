/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_select.h"
#include "vpath.h"

VMCmdSelect::VMCmdSelect( KarbonPart* part,
		const double tlX, const double tlY,
		const double brX, const double brY )
	: VCommand( part, i18n("Select Object") ), m_object( 0L ),
	  m_tlX( tlX ), m_tlY( tlY ), m_brX( brX ), m_brY( brY )
{
}

void
VMCmdSelect::execute()
{
	if ( m_object )
		m_object->setState( VObject::normal );
	else
		m_part->selectObjects(
			QRect( m_tlX, m_tlY, m_brX - m_tlX, m_brY - m_tlY ).normalize() );
}

void
VMCmdSelect::unexecute()
{
	if ( m_object )
		m_object->setState( VObject::normal );
}

VPath*
VMCmdSelect::createPath()
{
	VPath* path = new VPath();

	path->moveTo( m_tlX, m_tlY );
	path->lineTo( m_brX, m_tlY );
	path->lineTo( m_brX, m_brY );
	path->lineTo( m_tlX, m_brY );
	path->close();

	return path;
}

