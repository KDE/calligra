/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"

#include "karbon_view.h"
#include "vccmd_sinus.h"	// command
#include "vcdlg_sinus.h"	// dialog
#include "vctool_sinus.h"
#include "vpath.h"

VCToolSinus* VCToolSinus::s_instance = 0L;

VCToolSinus::VCToolSinus( KarbonPart* part )
	: VShapeTool( part )
{
	// create config dialog:
	m_dialog = new VCDlgSinus();
	m_dialog->setValueWidth( 100.0 );
	m_dialog->setValueHeight( 100.0 );
	m_dialog->setValuePeriods( 1 );
}

VCToolSinus::~VCToolSinus()
{
	delete( m_dialog );
}

VCToolSinus*
VCToolSinus::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolSinus( part );
	}

	return s_instance;
}

void
VCToolSinus::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	VPainter *painter = VPainterFactory::editpainter();
	
	VCCmdSinus* cmd =
		new VCCmdSinus( part(), p.x(), p.y(), p.x() + d1, p.y() + d2,
			m_dialog->valuePeriods() );

	VPath* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolSinus::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdSinus( part(),
					x, y,
					x + m_dialog->valueWidth(),
					y + m_dialog->valueHeight(),
					m_dialog->valuePeriods() );
		else
			return 0L;
	}
	else
		return
			new VCCmdSinus( part(),
				x, y,
				x + d1,
				y + d2,
				m_dialog->valuePeriods() );
}

