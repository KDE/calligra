/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"

#include "karbon_view.h"
#include "vccmd_ellipse.h"	// command
#include "vcdlg_ellipse.h"	// dialog
#include "vctool_ellipse.h"
#include "vpath.h"

VCToolEllipse* VCToolEllipse::s_instance = 0L;

VCToolEllipse::VCToolEllipse( KarbonPart* part )
	: VShapeTool( part )
{
	// create config dialog:
	m_dialog = new VCDlgEllipse();
	m_dialog->setValueWidth( 100.0 );
	m_dialog->setValueHeight( 100.0 );
}

VCToolEllipse::~VCToolEllipse()
{
	delete( m_dialog );
}

VCToolEllipse*
VCToolEllipse::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolEllipse( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VCToolEllipse::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();

	VCCmdEllipse* cmd =
		new VCCmdEllipse( part(), p.x(), p.y(), p.x() + d1, p.y() + d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolEllipse::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdEllipse( part(),
					x, y,
					x + m_dialog->valueWidth(),
					y + m_dialog->valueHeight() );
		else
			return 0L;
	}
	else
		return
			new VCCmdEllipse( part(),
				x, y,
				x + d1,
				y + d2 );
}
