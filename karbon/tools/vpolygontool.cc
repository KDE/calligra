/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vpolygoncmd.h"
#include "vpolygondlg.h"
#include "vpolygontool.h"


VPolygonTool* VPolygonTool::s_instance = 0L;

VPolygonTool::VPolygonTool( KarbonPart* part )
	: VShapeTool( part, true )
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

VPolygonTool*
VPolygonTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VPolygonTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VPolygonTool::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VPolygonCmd* cmd =
		new VPolygonCmd( &part()->document(),
			p.x(), p.y(),
			d1,
			m_dialog->edges(),
			d2 );

	VShape* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VPolygonTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VPolygonCmd( &part()->document(),
					x, y,
					m_dialog->radius(),
					m_dialog->edges() );
		else
			return 0L;
	}
	else
		return
			new VPolygonCmd( &part()->document(),
				x, y,
				d1,
				m_dialog->edges(),
				d2 );
}

void
VPolygonTool::showDialog() const
{
	m_dialog->exec();
}

