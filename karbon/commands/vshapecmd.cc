/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vdocument.h"
#include "vpath.h"
#include "vshapecmd.h"

VShapeCmd::VShapeCmd( VDocument* doc, const QString& name, VPath* path )
	: VCommand( doc, name ), m_path( path )
{
}

void
VShapeCmd::execute()
{
	if( !m_path )
		return;

	if( m_path->state() == state_deleted )
		m_path->setState( state_normal );
	else
	{
		m_doc->applyDefaultColors( *m_path );
		// Add path:
		m_doc->append( m_path );
		m_doc->select( *m_path, true );
	}
}

void
VShapeCmd::unexecute()
{
	if( !m_path )
		return;

	m_doc->deselect( *m_path );
	m_path->setState( state_deleted );
}

