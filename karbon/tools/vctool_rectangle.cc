/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"

#include "karbon_view.h"
#include "vccmd_rectangle.h"	// command
#include "vcdlg_rectangle.h"	// dialog
#include "vctool_rectangle.h"
#include "vpath.h"

VCToolRectangle* VCToolRectangle::s_instance = 0L;

VCToolRectangle::VCToolRectangle( KarbonPart* part )
	: VShapeTool( part )
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
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	VPainter *painter = VPainterFactory::editpainter();
	
	VCCmdRectangle* cmd =
		new VCCmdRectangle( part(), p.x(), p.y(), p.x() + d1, p.y() + d2 );

	VPath* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolRectangle::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VCCmdRectangle( part(),
					x, y,
					x + m_dialog->valueWidth(),
					y + m_dialog->valueHeight() );
		else
			return 0L;
	}
	else
		return
			new VCCmdRectangle( part(),
				x, y,
				x + d1,
				y + d2 );
}

