/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"

#include "karbon_view.h"
#include "vccmd_spiral.h"	// command
#include "vspiraldlg.h"	// dialog
#include "vctool_spiral.h"
#include "vpath.h"

VCToolSpiral* VCToolSpiral::s_instance = 0L;

VCToolSpiral::VCToolSpiral( KarbonPart* part )
	: VShapeTool( part, true )
{
	// create config dialog:
	m_dialog = new VSpiralDlg();
	m_dialog->setRadius( 100.0 );
	m_dialog->setSegments( 8 );
	m_dialog->setFade( 0.8 );
	m_dialog->setClockwise( true );
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

	s_instance->m_part = part;
	return s_instance;
}

void
VCToolSpiral::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VCCmdSpiral* cmd =
		new VCCmdSpiral( part(),
			p.x(), p.y(),
			d1,
			m_dialog->segments(),
			m_dialog->fade(),
			m_dialog->clockwise(),
			d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox( view->zoom() ) );

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
					m_dialog->radius(),
					m_dialog->segments(),
					m_dialog->fade(),
					m_dialog->clockwise(),
					d2 );
		else
			return 0L;
	}
	else
		return
			new VCCmdSpiral( part(),
				x, y,
				d1,
				m_dialog->segments(),
				m_dialog->fade(),
				m_dialog->clockwise(),
				d2 );
}

void
VCToolSpiral::showDialog() const
{
	m_dialog->exec();
}

