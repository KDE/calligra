/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vstarcmd.h"
#include "vstardlg.h"
#include "vstartool.h"


VStarTool* VStarTool::s_instance = 0L;

VStarTool::VStarTool( KarbonPart* part )
	: VShapeTool( part, true )
{
	// create config dialog:
	m_dialog = new VStarDlg();
	m_dialog->setOuterR( 100.0 );
	m_dialog->setInnerR( 50.0 );
	m_dialog->setEdges( 5 );
}

VStarTool::~VStarTool()
{
	delete( m_dialog );
}

VStarTool*
VStarTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VStarTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VStarTool::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VStarCmd* cmd =
		new VStarCmd( &part()->document(),
			p.x(), p.y(),
			d1,
			m_dialog->innerR() * d1 / m_dialog->outerR(),
			m_dialog->edges(),
			d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VStarTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VStarCmd( &part()->document(),
					x, y,
					m_dialog->outerR(),
					m_dialog->innerR(),
					m_dialog->edges() );
		else
			return 0L;
	}
	else
		return
			new VStarCmd( &part()->document(),
				x, y,
				d1,
				m_dialog->innerR() * d1 / m_dialog->outerR(),
				m_dialog->edges(),
				d2 );
}

void
VStarTool::showDialog() const
{
	m_dialog->exec();
}

