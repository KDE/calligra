/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vccmd_rectangle.h"	// command
#include "vcdlg_rectangle.h"	// dialog
#include "vctool_rectangle.h"
#include "vpath.h"

VCToolRectangle* VCToolRectangle::s_instance = 0L;

VCToolRectangle::VCToolRectangle( KarbonPart* part )
	: VTool( part )
{
	// create config dialog:
	m_dialog = new VCDlgRectangle();
	m_dialog->setValueWidth( 100.0 );
	m_dialog->setValueHeight( 100.0 );
}

VCToolRectangle::~VCToolRectangle()
{
	delete( m_dialog );
}

VCToolRectangle*
VCToolRectangle::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolRectangle( part );
	}

	return s_instance;
}

void
VCToolRectangle::drawTemporaryObject(
	KarbonView* view, const QPoint& tl, const QPoint& br )
{
	QPainter painter( view->canvasWidget()->viewport() );
	
	VCCmdRectangle* cmd =
		new VCCmdRectangle( part(), tl.x(), tl.y(), br.x(), br.y() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolRectangle::createCmdFromDialog( const QPoint& point )
{
	if ( m_dialog->exec() )
		return
			new VCCmdRectangle( part(),
				point.x(), point.y(),
				point.x() + m_dialog->valueWidth(),
				point.y() + m_dialog->valueHeight() );
	else
		return 0L;
}

VCommand*
VCToolRectangle::createCmdFromDragging( const QPoint& tl, const QPoint& br )
{
	return
		new VCCmdRectangle(
			part(), tl.x(), tl.y(), br.x(), br.y() );
}

