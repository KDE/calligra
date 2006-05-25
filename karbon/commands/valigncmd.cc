/* This file is doc of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#include "valigncmd.h"
#include "vtransformcmd.h"
#include "vdocument.h"
#include "vselection.h"

#include <kdebug.h>

VAlignCmd::VAlignCmd( VDocument *doc, Align align )
	: VCommand( doc, i18n( "Align Objects" ) ), m_align( align )
{
	m_trafoCmds.setAutoDelete( true );
}

VAlignCmd::~VAlignCmd()
{
}

void
VAlignCmd::execute()
{
	if( document()->selection()->objects().count() == 0 )
		return;
	double dx, dy;
	KoRect bbox;
	KoRect r;
	if( document()->selection()->objects().count() == 1 )
		r = document()->boundingBox();
	else
		r = document()->selection()->boundingBox();
	VObjectList objs = document()->selection()->objects();
	VObjectListIterator itr( objs );
	VTranslateCmd *trafoCmd = 0L;
	for( ; itr.current() ; ++itr )
	{
		document()->selection()->clear();
		bbox = itr.current()->boundingBox();
		switch( m_align )
		{
			case ALIGN_HORIZONTAL_LEFT	:	dx = r.topLeft().x() - bbox.topLeft().x(); dy = 0; break;
			case ALIGN_HORIZONTAL_CENTER:	dx = r.center().x() - bbox.center().x(); dy = 0; break;
			case ALIGN_HORIZONTAL_RIGHT	:	dx = r.topRight().x() - bbox.topRight().x(); dy = 0; break;
			case ALIGN_VERTICAL_TOP		:	dx = 0; dy = r.bottomRight().y() - bbox.bottomRight().y(); break;
			case ALIGN_VERTICAL_CENTER	:	dx = 0; dy = r.center().y() - bbox.center().y(); break;
			case ALIGN_VERTICAL_BOTTOM	:	dx = 0; dy = r.topLeft().y() - bbox.topLeft().y(); break;
		};
		document()->selection()->append( itr.current() );
		trafoCmd = new VTranslateCmd( document(), dx, dy );
		m_trafoCmds.append( trafoCmd );
		trafoCmd->execute();
	}
	itr.toFirst();
	for( ; itr.current() ; ++itr )
		document()->selection()->append( itr.current() );
	setSuccess( true );
}

void
VAlignCmd::unexecute()
{
	Q3PtrListIterator<VTranslateCmd> itr( m_trafoCmds );
	for( ; itr.current() ; ++itr )
		itr.current()->unexecute();
	setSuccess( false );
}

