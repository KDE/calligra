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
#include "vpolygon.h"
#include "vpolygondlg.h"
#include "vpolygontool.h"


VPolygonTool::VPolygonTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Polygon" ), true )
{
	// create config dialog:
	m_dialog = new VPolygonDlg();
	m_dialog->setRadius( 100.0 );
	m_dialog->setEdges( 5 );
}

VPolygonTool::~VPolygonTool()
{
	delete( m_dialog );
}

VComposite*
VPolygonTool::shape( bool interactive ) const
{
	if( interactive )
	{
		if( m_dialog->exec() )
			return
				new VPolygon(
					0L,
					m_p,
					m_dialog->radius(),
					m_dialog->edges() );
		else
			return 0L;
	}
	else
		return
			new VPolygon(
				0L,
				m_p,
				m_d1,
				m_dialog->edges(),
				m_d2 );
}

void
VPolygonTool::showDialog() const
{
	m_dialog->exec();
}

