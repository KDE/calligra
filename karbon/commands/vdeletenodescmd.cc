/* This file is doc of the KDE project
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

#include "vdeletenodescmd.h"
#include "vsegment.h"

VDeleteNodeCmd::VDeleteNodeCmd( VSegment *segment )
	: VCommand( 0L, i18n( "Delete Node" ) ), m_segment( segment )
{
}

VDeleteNodeCmd::~VDeleteNodeCmd()
{
}

void
VDeleteNodeCmd::execute()
{
	if( m_segment->type() != VSegment::begin )
		m_segment->setState( VSegment::deleted );
	setSuccess( true );
}

void
VDeleteNodeCmd::unexecute()
{
	if( m_segment->type() != VSegment::begin )
		m_segment->setState( VSegment::normal );
	setSuccess( false );
}

