/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_view.h"
#include "vsinus.h"
#include "vsinusdlg.h"
#include "vsinustool.h"


VSinusTool::VSinusTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Sinus" ) )
{
	// create config dialog:
	m_dialog = new VSinusDlg();
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
	m_dialog->setPeriods( 1 );
}

VSinusTool::~VSinusTool()
{
	delete( m_dialog );
}

void
VSinusTool::activate()
{
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

VPath*
VSinusTool::shape() const
{
	if( m_d1 <= 1.0 && m_d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VSinus(
					0L,
					m_p,
					m_dialog->width(),
					m_dialog->height(),
					m_dialog->periods() );
		else
			return 0L;
	}
	else
		return
			new VSinus(
				0L,
				m_p,
				m_d1,
				m_d2,
				m_dialog->periods() );
}

void
VSinusTool::showDialog() const
{
	m_dialog->exec();
}

