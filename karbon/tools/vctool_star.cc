/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_star.h"	// command
#include "vcdlg_star.h"	// dialog
#include "vctool_star.h"
#include "vpath.h"

VCToolStar* VCToolStar::s_instance = 0L;

VCToolStar::VCToolStar( KarbonPart* part )
	: VShapeTool( part, true )
{
	// create config dialog:
	m_dialog = new VCDlgStar();
	m_dialog->setValueOuterR( 100.0 );
	m_dialog->setValueInnerR( 50.0 );
	m_dialog->setValueEdges( 5 );
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

	return s_instance;
}

void
VCToolStar::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdStar* cmd =
		new VCCmdStar( part(),
			p.x(), p.y(),
			d1,
			m_dialog->valueInnerR() * d1 / m_dialog->valueOuterR(),
			m_dialog->valueEdges(),
			d2 );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox( view->zoomFactor() ) );

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
					m_dialog->valueOuterR(),
					m_dialog->valueInnerR(),
					m_dialog->valueEdges() );
		else
			return 0L;
	}
	else
		return
			new VCCmdStar( part(),
				x, y,
				d1,
				m_dialog->valueInnerR() * d1 / m_dialog->valueOuterR(),
				m_dialog->valueEdges(),
				d2 );
}

