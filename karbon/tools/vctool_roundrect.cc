/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_roundrect.h"	// command
#include "vcdlg_roundrect.h"	// dialog
#include "vctool_roundrect.h"
#include "vpath.h"

VCToolRoundRect* VCToolRoundRect::s_instance = 0L;

VCToolRoundRect::VCToolRoundRect( KarbonPart* part )
	: VTool( part )
{
	// create config dialog:
	m_dialog = new VCDlgRoundRect();
	m_dialog->setValueWidth( 100.0 );
	m_dialog->setValueHeight( 100.0 );
	m_dialog->setValueRound( 20.0 );
}

VCToolRoundRect::~VCToolRoundRect()
{
	delete( m_dialog );
}

VCToolRoundRect*
VCToolRoundRect::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolRoundRect( part );
	}

	return s_instance;
}

void
VCToolRoundRect::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdRoundRect* cmd =
		new VCCmdRoundRect( part(), p.x(), p.y(), p.x() + d1, p.y() + d2,
			m_dialog->valueRound() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolRoundRect::createCmd( const QPoint& p, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdRoundRect( part(),
					p.x(), p.y(),
					p.x() + m_dialog->valueWidth(),
					p.y() + m_dialog->valueHeight(),
					m_dialog->valueRound() );
		else
			return 0L;
	}
	else
		return
			new VCCmdRoundRect( part(),
				p.x(), p.y(),
				p.x() + d1,
				p.y() + d2,
				m_dialog->valueRound() );
}
