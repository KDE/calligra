/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_polygon.h"	// command
#include "vcdlg_polygon.h"	// dialog
#include "vctool_polygon.h"
#include "vpath.h"

VCToolPolygon* VCToolPolygon::s_instance = 0L;

VCToolPolygon::VCToolPolygon( KarbonPart* part )
	: VTool( part, true )
{
	// create config dialog:
	m_dialog = new VCDlgPolygon();
	m_dialog->setValueRadius( 100.0 );
	m_dialog->setValueEdges( 5 );
}

VCToolPolygon::~VCToolPolygon()
{
	delete( m_dialog );
}

VCToolPolygon*
VCToolPolygon::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolPolygon( part );
	}

	return s_instance;
}

void
VCToolPolygon::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdPolygon* cmd =
		new VCCmdPolygon( part(),
			p.x(), p.y(),
			d1,
			m_dialog->valueEdges(),
			d2 );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolPolygon::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdPolygon( part(),
					x, y,
					m_dialog->valueRadius(),
					m_dialog->valueEdges() );
		else
			return 0L;
	}
	else
		return
			new VCCmdPolygon( part(),
				x, y,
				d1,
				m_dialog->valueEdges(),
				d2 );
}

