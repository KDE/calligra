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

#include <klocale.h>

#include "vfill.h"
#include "vfillcmd.h"
#include "vselection.h"


VFillCmd::VFillCmd( VDocument *doc, const VFill &fill )
	: VCommand( doc, i18n( "Fill Objects" ) ), m_fill( fill )
{
	m_selection = document()->selection()->clone();

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Fill Object" ) );
}

VFillCmd::~VFillCmd()
{
	delete( m_selection );
}

void
VFillCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		//if( m_opacity == -1 )
		//	m_color.setOpacity( itr.current()->fill().color().opacity() );

		m_oldcolors.push_back( VFill( *itr.current()->fill() ) );

		itr.current()->setFill( m_fill );
	}
}

void
VFillCmd::unexecute()
{
	VObjectListIterator itr( m_selection->objects() );
	int i = 0;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setFill( m_oldcolors[ i++ ] );
	}
}

