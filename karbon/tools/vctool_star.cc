/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_star.h"	// command
#include "vcdlg_star.h"	// dialog
#include "vctool_star.h"
#include "vpath.h"

VCToolStar* VCToolStar::s_instance = 0L;

VCToolStar::VCToolStar( KarbonPart* part )
	: VTool( part )
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
	KarbonView* view, const QPoint& tl, const QPoint& br )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdStar* cmd =
		new VCCmdStar( part(), tl.x(), tl.y(), br.x(), br.y(),
			m_dialog->valueEdges() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolStar::createCmdFromDialog( const QPoint& point )
{
	if ( m_dialog->exec() )
		return
			new VCCmdStar( part(), point.x(), point.y(),
				point.x() + m_dialog->valueOuterR(),
				point.y() + m_dialog->valueInnerR(),
				m_dialog->valueEdges() );
	else
		return 0L;
}

VCommand*
VCToolStar::createCmdFromDragging( const QPoint& tl, const QPoint& br )
{
	return
		new VCCmdStar(
			part(), tl.x(), tl.y(), br.x(), br.y(), m_dialog->valueEdges() );
}

