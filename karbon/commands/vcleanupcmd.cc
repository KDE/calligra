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

#include "vcleanupcmd.h"
#include "vlayer.h"


VCleanUpCmd::VCleanUpCmd( VDocument *doc )
		: VCommand( doc, i18n( "Clean Up" ) )
{
}

VCleanUpCmd::~VCleanUpCmd()
{
}

void
VCleanUpCmd::execute()
{
	visit( *document() );
}

void
VCleanUpCmd::unexecute()
{
}

bool
VCleanUpCmd::isExecuted()
{
		// TODO: change when implementing the unexecute method.
	return true;
}

void
VCleanUpCmd::visitVLayer( VLayer& layer )
{
	VObjectListIterator itr( layer.objects() );
	for( ; itr.current(); ++itr )
	{
		if( itr.current()->state() == VObject::deleted )
		{
			delete( itr.current() );
			layer.take( *itr.current() );
		}
	}
}
