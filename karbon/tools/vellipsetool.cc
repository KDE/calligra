/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "vellipsecmd.h"
#include "vellipsedlg.h"
#include "vellipsetool.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"


VEllipseTool::VEllipseTool( KarbonView* view )
	: VShapeTool( view )
{
	// create config dialog:
	m_dialog = new VEllipseDlg();
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
}

VEllipseTool::~VEllipseTool()
{
	delete( m_dialog );
}

void
VEllipseTool::activate()
{
	view()->statusMessage()->setText( i18n( "Insert Ellipse" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

void
VEllipseTool::drawTemporaryObject( const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view()->painterFactory()->editpainter();

	VEllipseCmd* cmd =
		new VEllipseCmd( &view()->part()->document(), p.x(), p.y(), p.x() + d1, p.y() + d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VEllipseTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VEllipseCmd( &view()->part()->document(),
					x, y,
					x + m_dialog->width(),
					y + m_dialog->height() );
		else
			return 0L;
	}
	else
		return
			new VEllipseCmd( &view()->part()->document(),
				x, y,
				x + d1,
				y + d2 );
}

