/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_ellipse.h"	// command
#include "vcdlg_ellipse.h"	// dialog
#include "vctool_ellipse.h"
#include "vpath.h"

VCToolEllipse* VCToolEllipse::s_instance = 0L;

VCToolEllipse::VCToolEllipse( KarbonPart* part )
	: VTool( part )
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

	return s_instance;
}

void
VCToolEllipse::drawTemporaryObject(
	KarbonView* view, const QPoint& tl, const QPoint& br )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdEllipse* cmd =
		new VCCmdEllipse( part(), tl.x(), tl.y(), br.x(), br.y() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolEllipse::createCmdFromDialog( const QPoint& point )
{
	if ( m_dialog->exec() )
		return
			new VCCmdEllipse( part(),
				point.x(), point.y(),
				point.x() + m_dialog->valueWidth(),
				point.y() + m_dialog->valueHeight() );
	else
		return 0L;
}

VCommand*
VCToolEllipse::createCmdFromDragging( const QPoint& tl, const QPoint& br )
{
	return
		new VCCmdEllipse(
			part(), tl.x(), tl.y(), br.x(), br.y() );
}

