/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vctool_roundrect.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vroundrectcmd.h"	// command
#include "vroundrectdlg.h"	// dialog


VCToolRoundRect* VCToolRoundRect::s_instance = 0L;

VCToolRoundRect::VCToolRoundRect( KarbonPart* part )
	: VShapeTool( part )
{
	// create config dialog:
	m_dialog = new VRoundRectDlg();
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
	m_dialog->setRound( 20.0 );
}

VCToolRoundRect::~VCToolRoundRect()
{
	delete( m_dialog );
}

VCToolRoundRect*
VCToolRoundRect::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolRoundRect( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VCToolRoundRect::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VRoundRectCmd* cmd =
		new VRoundRectCmd( part(), p.x(), p.y(), p.x() + d1, p.y() + d2,
			m_dialog->round() );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolRoundRect::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VRoundRectCmd( part(),
					x, y,
					x + m_dialog->width(),
					y + m_dialog->height(),
					m_dialog->round() );
		else
			return 0L;
	}
	else
		return
			new VRoundRectCmd( part(),
				x, y,
				x + d1,
				y + d2,
				m_dialog->round() );
}

void
VCToolRoundRect::showDialog() const
{
	m_dialog->exec();
}

