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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>

#include "vcleanupcmd.h"
#include "vlayer.h"
#include "vdocument.h"

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

void
VCleanUpCmd::visitVLayer( KoLayerShape& layer )
{
	// TODO: is that absolete now?
	/*
	VObjectListIterator itr( layer.objects() );
	for( ; itr.current(); ++itr )
	{
		if( itr.current()->state() == VObject::deleted )
		{
			delete( itr.current() );
			layer.take( *itr.current() );
		}
	}
	*/
}
