/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vellipse.h"
#include "vellipsedlg.h"
#include "vellipsetool.h"


VEllipseTool::VEllipseTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Ellipse" ) )
{
	// create config dialog:
	m_dialog = new VEllipseDlg(view->part());
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
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

VPath*
VEllipseTool::shape( bool decide ) const
{
	if( decide && m_d1 <= 1.0 && m_d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VEllipse(
					0L,
					m_p,
					m_dialog->width(),
					m_dialog->height() );
		else
			return 0L;
	}
	else
		return
			new VEllipse(
				0L,
				m_p,
				m_d1,
				m_d2 );
}
