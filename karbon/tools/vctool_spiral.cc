/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_spiral.h"	// command
#include "vcdlg_spiral.h"	// dialog
#include "vctool_spiral.h"
#include "vpath.h"

VCToolSpiral* VCToolSpiral::s_instance = 0L;

VCToolSpiral::VCToolSpiral( KarbonPart* part )
	: VTool( part, true )
{
	// create config dialog:
	m_dialog = new VCDlgSpiral();
	m_dialog->setValueRadius( 100.0 );
	m_dialog->setValueSegments( 8 );
	m_dialog->setValueFade( 0.8 );
	m_dialog->setValueClockwise( true );
}

VCToolSpiral::~VCToolSpiral()
{
	delete( m_dialog );
}

VCToolSpiral*
VCToolSpiral::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolSpiral( part );
	}

	return s_instance;
}

void
VCToolSpiral::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdSpiral* cmd =
		new VCCmdSpiral( part(),
			p.x(), p.y(),
			d1,
			m_dialog->valueSegments(),
			m_dialog->valueFade(),
			m_dialog->valueClockwise(),
			d2 );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolSpiral::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdSpiral( part(),
					x, y,
					m_dialog->valueRadius(),
					m_dialog->valueSegments(),
					m_dialog->valueFade(),
					m_dialog->valueClockwise(),
					d2 );
		else
			return 0L;
	}
	else
		return
			new VCCmdSpiral( part(),
				x, y,
				d1,
				m_dialog->valueSegments(),
				m_dialog->valueFade(),
				m_dialog->valueClockwise(),
				d2 );
}
