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

#include "karbon_view.h"
#include "karbon_part.h"
#include "vellipse.h"
#include "vellipsedlg.h"
#include "vellipsetool.h"


VEllipseTool::VEllipseTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Ellipse" ) )
{
	// create config dialog:
	m_dialog = new VEllipseDlg(view->part());
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
}

VEllipseTool::~VEllipseTool()
{
	delete( m_dialog );
}

void VEllipseTool::refreshUnit()
{
    m_dialog->refreshUnit();
}

VPath*
VEllipseTool::shape( bool interactive ) const
{
	if( interactive )
	{
		if( m_dialog->exec() )
			return
				new VEllipse(
					0L,
					m_p,
					m_dialog->width(),
					m_dialog->height() );
		else
			return 0L;
	}
	else
		return
			new VEllipse(
				0L,
				m_p,
				m_d1,
				m_d2 );
}
