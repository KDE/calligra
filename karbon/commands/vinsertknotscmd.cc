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

#include "vinsertknotscmd.h"
#include "vpath.h"
#include "vsegment.h"
#include "vselection.h"


VInsertKnotsCmd::VInsertKnotsCmd( VDocument *doc, uint knots )
		: VCommand( doc, i18n( "Insert Knots" ) )
{
	m_selection = m_doc->selection()->clone();

	m_knots = knots > 0 ? knots : 1;
}

VInsertKnotsCmd::~VInsertKnotsCmd()
{
	delete( m_selection );
}

void
VInsertKnotsCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );

	for ( ; itr.current() ; ++itr )
		visit( *itr.current() );
}

void
VInsertKnotsCmd::unexecute()
{
}

void
VInsertKnotsCmd::visitVPath( VPath& path )
{
	path.first();

	// ommit "begin" segment:
	while( path.next() )
	{
		for( uint i = m_knots; i > 0; --i )
		{
			path.insert(
				path.current()->splitAt( 1.0 / ( i + 1.0 ) ) );

			path.next();
		}

		if( !success() )
			setSuccess();
	}
}
