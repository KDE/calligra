/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vsinuscmd.h"
#include "vsinusdlg.h"
#include "vsinustool.h"


VSinusTool* VSinusTool::s_instance = 0L;

VSinusTool::VSinusTool( KarbonPart* part )
	: VShapeTool( part )
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

VSinusTool*
VSinusTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VSinusTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VSinusTool::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VSinusCmd* cmd =
		new VSinusCmd( &part()->document(), p.x(), p.y(), p.x() + d1, p.y() + d2,
			m_dialog->periods() );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VSinusTool::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VSinusCmd( &part()->document(),
					x, y,
					x + m_dialog->width(),
					y + m_dialog->height(),
					m_dialog->periods() );
		else
			return 0L;
	}
	else
		return
			new VSinusCmd( &part()->document(),
				x, y,
				x + d1,
				y + d2,
				m_dialog->periods() );
}

void
VSinusTool::showDialog() const
{
	m_dialog->exec();
}

