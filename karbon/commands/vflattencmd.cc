/* This file is part of the KDE project
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

#include <klocale.h>

#include "vflatten.h"
#include "vflattencmd.h"
#include "vgroup.h"
#include "vselection.h"


VFlattenCmd::VFlattenCmd( VDocument *doc, double flatness )
		: VCommand( doc, i18n( "Flatten" ) )
{
	m_selection = m_doc->selection()->clone();

	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

VFlattenCmd::~VFlattenCmd()
{
	delete( m_selection );
}

void
VFlattenCmd::execute()
{
	VObject* newObject;

	VFlatten op( m_flatness );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
/*		if( VGroup* group = dynamic_cast<VGroup*>( itr.current()->parent() ) )
		{
			newObject = itr.current()->clone();

			group->insertNewDeleteOld( newObject, itr.current() );
		}
		else
		{
*/
			newObject = itr.current();
//		}

		op.visit( *newObject );
	}
}

void
VFlattenCmd::unexecute()
{
}

