/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_sinus.h"	// command
#include "vcdlg_sinus.h"	// dialog
#include "vctool_sinus.h"
#include "vpath.h"

VCToolSinus* VCToolSinus::s_instance = 0L;

VCToolSinus::VCToolSinus( KarbonPart* part )
	: VTool( part )
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
	KarbonView* view, const QPoint& tl, const QPoint& br )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdSinus* cmd =
		new VCCmdSinus( part(), tl.x(), tl.y(), br.x(), br.y(),
			m_dialog->valuePeriods() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolSinus::createCmdFromDialog( const QPoint& point )
{
	if ( m_dialog->exec() )
		return
			new VCCmdSinus( part(), point.x(), point.y(),
				point.x() + m_dialog->valueWidth(),
				point.y() + m_dialog->valueHeight(),
				m_dialog->valuePeriods() );
	else
		return 0L;
}

VCommand*
VCToolSinus::createCmdFromDragging( const QPoint& tl, const QPoint& br )
{
	return
		new VCCmdSinus(
			part(), tl.x(), tl.y(), br.x(), br.y(), m_dialog->valuePeriods() );
}

