/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "vroundrect.h"
#include "vroundrectdlg.h"
#include "vroundrecttool.h"
#include "karbon_part.h"


VRoundRectTool::VRoundRectTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Round Rectangle" ) )
{
	// create config dialog:
	m_dialog = new VRoundRectDlg(view->part());
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
	m_dialog->setRound( 20.0 );
}

VRoundRectTool::~VRoundRectTool()
{
	delete( m_dialog );
}

void
VRoundRectTool::activate()
{
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

VPath*
VRoundRectTool::shape( bool decide ) const
{
	if( decide && m_d1 <= 1.0 && m_d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VRoundRect(
					0L,
					m_p,
					m_dialog->width(),
					m_dialog->height(),
					m_dialog->round() );
		else
			return 0L;
	}
	else
		return
			new VRoundRect(
				0L,
				m_p,
				m_d1,
				m_d2,
				m_dialog->round() );
}

void
VRoundRectTool::showDialog() const
{
	m_dialog->exec();
}

