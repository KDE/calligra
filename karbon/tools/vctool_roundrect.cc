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
	KarbonView* view, const QPoint& tl, const QPoint& br )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdRoundRect* cmd =
		new VCCmdRoundRect( part(), tl.x(), tl.y(), br.x(), br.y(),
			m_dialog->valueRound() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolRoundRect::createCmdFromDialog( const QPoint& point )
{
	if ( m_dialog->exec() )
		return
			new VCCmdRoundRect( part(), point.x(), point.y(),
				point.x() + m_dialog->valueWidth(),
				point.y() + m_dialog->valueHeight(),
				m_dialog->valueRound() );
	else
		return 0L;
}

VCommand*
VCToolRoundRect::createCmdFromDragging( const QPoint& tl, const QPoint& br )
{
	return
		new VCCmdRoundRect( part(), tl.x(), tl.y(), br.x(), br.y(),
				m_dialog->valueRound() );
}

