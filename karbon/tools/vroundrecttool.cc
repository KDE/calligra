/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vroundrectcmd.h"
#include "vroundrectdlg.h"
#include "vroundrecttool.h"


VRoundRectTool* VRoundRectTool::s_instance = 0L;

VRoundRectTool::VRoundRectTool( KarbonPart* part )
	: VShapeTool( part )
{
	// create config dialog:
	m_dialog = new VRoundRectDlg();
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
	m_dialog->setRound( 20.0 );
}

VRoundRectTool::~VRoundRectTool()
{
	delete( m_dialog );
}

VRoundRectTool*
VRoundRectTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VRoundRectTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VRoundRectTool::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VRoundRectCmd* cmd =
		new VRoundRectCmd( &part()->document(), p.x(), p.y(), p.x() + d1, p.y() + d2,
			m_dialog->round() );

	VShape* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VRoundRectTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VRoundRectCmd( &part()->document(),
					x, y,
					x + m_dialog->width(),
					y + m_dialog->height(),
					m_dialog->round() );
		else
			return 0L;
	}
	else
		return
			new VRoundRectCmd( &part()->document(),
				x, y,
				x + d1,
				y + d2,
				m_dialog->round() );
}

void
VRoundRectTool::showDialog() const
{
	m_dialog->exec();
}

