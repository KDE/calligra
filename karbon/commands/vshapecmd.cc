/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "vcomposite.h"
#include "vdocument.h"
#include "vselection.h"
#include "vshapecmd.h"


VShapeCmd::VShapeCmd( VDocument* doc, const QString& name, VComposite* composite )
	: VCommand( doc, name ), m_composite( composite )
{
}

void
VShapeCmd::execute()
{
	if( !m_composite )
		return;

	if( m_composite->state() == VObject::deleted )
		m_composite->setState( VObject::normal );
	else
	{
		m_composite->setState( VObject::normal );
		m_doc->applyDefaultColors( *m_composite );

		// Add path:
		m_doc->append( m_composite );
		m_doc->selection()->clear();
		m_doc->selection()->append( m_composite );
	}
}

void
VShapeCmd::unexecute()
{
	if( !m_composite )
		return;

	m_doc->selection()->take( *m_composite );
	m_composite->setState( VObject::deleted );
}

