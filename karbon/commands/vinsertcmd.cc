/* This file is part of the KDE project
   Copyright (C) 2005, Inge Wallin

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <klocale.h>

#include "vdocument.h"
#include "vlayer.h"
#include "vselection.h"
#include "vinsertcmd.h"
#include "vtransformcmd.h"


VInsertCmd::VInsertCmd( VDocument *doc, const QString& name,
						VObjectList *objects, 
						double offset )
	: VCommand( doc, name, "14_insert" ),
	  m_objects( *objects ),
	  m_offset( offset )
{
}

VInsertCmd::~VInsertCmd()
{
}


void
VInsertCmd::execute()
{
	VObjectListIterator itr( m_objects );

	document()->selection()->clear();
	for ( ; itr.current() ; ++itr ) {
		VObject  *object = itr.current();

		if ( object->state() == VObject::deleted ) {
			object->setState( VObject::normal );
		}
		else {
			// TODO: porting to flake
			//document()->append( object );

			if ( m_offset != 0.0 ) {
				VTranslateCmd  cmd( 0L, m_offset, -m_offset );
				cmd.visit( *object );
			}
		}

		document()->selection()->append( object );
	}

	setSuccess( true );
}


void
VInsertCmd::unexecute()
{
	document()->selection()->clear();

	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )	{
		itr.current()->setState( VObject::deleted );
	}

	setSuccess( false );
}

