/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "vspiral.h"
#include "vspiraldlg.h"
#include "vspiraltool.h"


VSpiralTool::VSpiralTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Spiral" ), true )
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
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

VPath*
VSpiralTool::shape( bool interactive ) const
{
	if( interactive && m_d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VSpiral(
					0L,
					m_p,
					m_dialog->radius(),
					m_dialog->segments(),
					m_dialog->fade(),
					m_dialog->clockwise(),
					m_d2 );
		else
			return 0L;
	}
	else
		return
			new VSpiral(
				0L,
				m_p,
				m_d1,
				m_dialog->segments(),
				m_dialog->fade(),
				m_dialog->clockwise(),
				m_d2 );
}

void
VSpiralTool::showDialog() const
{
	m_dialog->exec();
}

