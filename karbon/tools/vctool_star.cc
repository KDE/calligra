/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"

#include "karbon_view.h"
#include "vccmd_star.h"	// command
#include "vstardlg.h"	// dialog
#include "vctool_star.h"
#include "vpath.h"

VCToolStar* VCToolStar::s_instance = 0L;

VCToolStar::VCToolStar( KarbonPart* part )
	: VShapeTool( part, true )
{
	// create config dialog:
	m_dialog = new VStarDlg();
	m_dialog->setOuterR( 100.0 );
	m_dialog->setInnerR( 50.0 );
	m_dialog->setEdges( 5 );
}

VCToolStar::~VCToolStar()
{
	delete( m_dialog );
}

VCToolStar*
VCToolStar::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolStar( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VCToolStar::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VCCmdStar* cmd =
		new VCCmdStar( part(),
			p.x(), p.y(),
			d1,
			m_dialog->innerR() * d1 / m_dialog->outerR(),
			m_dialog->edges(),
			d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox( view->zoom() ) );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolStar::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdStar( part(),
					x, y,
					m_dialog->outerR(),
					m_dialog->innerR(),
					m_dialog->edges() );
		else
			return 0L;
	}
	else
		return
			new VCCmdStar( part(),
				x, y,
				d1,
				m_dialog->innerR() * d1 / m_dialog->outerR(),
				m_dialog->edges(),
				d2 );
}

void
VCToolStar::showDialog() const
{
	m_dialog->exec();
}

