/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "vstar.h"
#include "vstardlg.h"
#include "vstartool.h"


VStarTool::VStarTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Star" ), true )
{
	// create config dialog:
	m_dialog = new VStarDlg(view->part());
	m_dialog->setOuterR( 100.0 );
	m_dialog->setInnerR( 50.0 );
	m_dialog->setEdges( 5 );
}

VStarTool::~VStarTool()
{
	delete( m_dialog );
}

void
VStarTool::activate()
{
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

VPath*
VStarTool::shape( bool decide ) const
{
	if( decide && m_d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VStar(
					0L,
					m_p,
					m_dialog->outerR(),
					m_dialog->innerR(),
					m_dialog->edges() );
		else
			return 0L;
	}
	else
		return
			new VStar(
				0L,
				m_p,
				m_d1,
				m_dialog->innerR() * m_d1 / m_dialog->outerR(),
				m_dialog->edges(),
				m_d2 );
}

void
VStarTool::showDialog() const
{
	m_dialog->exec();
}

