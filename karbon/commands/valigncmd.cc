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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
	m_trafoCmd = 0L;
}

VAlignCmd::~VAlignCmd()
{
	delete( m_trafoCmd );
}

void
VAlignCmd::execute()
{
	double dx, dy;
	KoRect r = document()->selection()->boundingBox();
	double docWidth = document()->width();
	double docHeight= document()->height();
	double rectCenterX = r.center().x();
	double rectCenterY = r.center().y();
	switch( m_align )
	{
		case ALIGN_TOPLEFT		:	dx = - r.topLeft().x(); dy = ( docHeight ) - rectCenterY; break;
		case ALIGN_TOPCENTER	:	dx = ( docWidth / 2.0 ) - rectCenterX; dy = ( docHeight ) - rectCenterY; break;
		case ALIGN_TOPRIGHT		:	dx = docWidth - r.topRight().x(); dy = ( docHeight ) - rectCenterY; break;
		case ALIGN_BOTTOMLEFT	:	break;
		case ALIGN_BOTTOMCENTER	:	dx = ( docWidth / 2.0 ) - rectCenterX; break;
		case ALIGN_BOTTOMRIGHT	:	break;
		case ALIGN_CENTERLEFT	:	dy = ( docHeight / 2.0 )  - rectCenterY; break;
		case ALIGN_CENTERRIGHT	:	dy = ( docHeight / 2.0 )  - rectCenterY; break;
		default:
			dx = ( docWidth / 2.0 )  - rectCenterX;
			dy = ( docHeight / 2.0 ) - rectCenterY;
	};
	m_trafoCmd = new VTranslateCmd( document(), dx, dy );
	kdDebug() << dx << ", " << dy << endl;
	m_trafoCmd->execute();
	setSuccess( true );
}

void
VAlignCmd::unexecute()
{
	m_trafoCmd->unexecute();
	setSuccess( false );
}

