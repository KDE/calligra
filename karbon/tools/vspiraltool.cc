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
#include "vspiralcmd.h"
#include "vspiraldlg.h"
#include "vspiraltool.h"


VSpiralTool::VSpiralTool( KarbonView* view )
	: VShapeTool( view, true )
{
	// create config dialog:
	m_dialog = new VSpiralDlg();
	m_dialog->setRadius( 100.0 );
	m_dialog->setSegments( 8 );
	m_dialog->setFade( 0.8 );
	m_dialog->setClockwise( true );
}

VSpiralTool::~VSpiralTool()
{
	delete( m_dialog );
}

void
VSpiralTool::activate()
{
	view()->statusMessage()->setText( i18n( "Insert Spiral" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

void
VSpiralTool::drawTemporaryObject( const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view()->painterFactory()->editpainter();
	
	VSpiralCmd* cmd =
		new VSpiralCmd( &view()->part()->document(),
			p.x(), p.y(),
			d1,
			m_dialog->segments(),
			m_dialog->fade(),
			m_dialog->clockwise(),
			d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VSpiralTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VSpiralCmd( &view()->part()->document(),
					x, y,
					m_dialog->radius(),
					m_dialog->segments(),
					m_dialog->fade(),
					m_dialog->clockwise(),
					d2 );
		else
			return 0L;
	}
	else
		return
			new VSpiralCmd( &view()->part()->document(),
				x, y,
				d1,
				m_dialog->segments(),
				m_dialog->fade(),
				m_dialog->clockwise(),
				d2 );
}

void
VSpiralTool::showDialog() const
{
	m_dialog->exec();
}

