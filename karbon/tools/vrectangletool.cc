/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vrectanglecmd.h"
#include "vrectangledlg.h"
#include "vrectangletool.h"


VRectangleTool::VRectangleTool( KarbonView* view )
	: VShapeTool( view )
{
	// create config dialog:
	m_dialog = new VRectangleDlg();
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
}

VRectangleTool::~VRectangleTool()
{
	delete( m_dialog );
}

void
VRectangleTool::activate()
{
	view()->statusMessage()->setText( i18n( "Insert Rectangle" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

void
VRectangleTool::drawTemporaryObject( const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view()->painterFactory()->editpainter();
	
	VRectangleCmd* cmd =
		new VRectangleCmd( &view()->part()->document(), p.x(), p.y(), p.x() + d1, p.y() + d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VRectangleTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VRectangleCmd( &view()->part()->document(),
					x, y,
					x + m_dialog->width(),
					y + m_dialog->height() );
		else
			return 0L;
	}
	else
		return
			new VRectangleCmd( &view()->part()->document(),
				x, y,
				x + d1,
				y + d2 );
}

