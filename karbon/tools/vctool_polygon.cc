/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vctool_polygon.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vpolygoncmd.h"	// command
#include "vpolygondlg.h"	// dialog


VCToolPolygon* VCToolPolygon::s_instance = 0L;

VCToolPolygon::VCToolPolygon( KarbonPart* part )
	: VShapeTool( part, true )
{
	// create config dialog:
	m_dialog = new VPolygonDlg();
	m_dialog->setRadius( 100.0 );
	m_dialog->setEdges( 5 );
}

VCToolPolygon::~VCToolPolygon()
{
	delete( m_dialog );
}

VCToolPolygon*
VCToolPolygon::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolPolygon( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VCToolPolygon::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();
	
	VPolygonCmd* cmd =
		new VPolygonCmd( &part()->document(),
			p.x(), p.y(),
			d1,
			m_dialog->edges(),
			d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolPolygon::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VPolygonCmd( &part()->document(),
					x, y,
					m_dialog->radius(),
					m_dialog->edges() );
		else
			return 0L;
	}
	else
		return
			new VPolygonCmd( &part()->document(),
				x, y,
				d1,
				m_dialog->edges(),
				d2 );
}

void
VCToolPolygon::showDialog() const
{
	m_dialog->exec();
}

