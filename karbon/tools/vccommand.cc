/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vccommand.h"
#include <vpath.h>

void
VCCommand::execute()
{
	if ( m_object )
		m_object->setState( VObject::normal );
	else
	{
		m_object = createPath();
		// add path:
		m_part->unselectObjects();
		m_part->insertObject( m_object );
		m_object->setState( VObject::selected );
	}
}

void
VCCommand::unexecute()
{
	if ( m_object )
		m_object->setState( VObject::deleted );
}

