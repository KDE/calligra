/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vdocument.h"
#include "vobject.h"
#include "vshapecmd.h"

void
VShapeCmd::execute()
{
	if ( m_object )
		m_object->setState( state_normal );
	else
	{
		m_object = createPath();

		m_doc->applyDefaultColors( *m_object );
		// add path:
		m_doc->append( m_object );
		m_doc->select( *m_object, true );
	}
}

void
VShapeCmd::unexecute()
{
	if ( m_object )
	{
		m_doc->deselect( *m_object );
		m_object->setState( state_deleted );
	}
}

