/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "vrectangle.h"
#include "vrectangledlg.h"
#include "vrectangletool.h"


VRectangleTool::VRectangleTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Rectangle" ) )
{
	// create config dialog:
	m_dialog = new VRectangleDlg(view->part());
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
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

VPath*
VRectangleTool::shape( bool decide ) const
{
	if( decide && m_d1 <= 1.0 && m_d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VRectangle(
					0L,
					m_p,
					m_dialog->width(),
					m_dialog->height() );
		else
			return 0L;
	}
	else
		return
			new VRectangle(
				0L,
				m_p,
				m_d1,
				m_d2 );
}

