/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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

VPath*
VPolygonTool::shape( bool interactive ) const
{
	if( interactive && m_d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
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

