/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_view.h"
#include "vctool_ellipse.h"
#include "vellipsecmd.h"	// command
#include "vellipsedlg.h"	// dialog
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"


VCToolEllipse* VCToolEllipse::s_instance = 0L;

VCToolEllipse::VCToolEllipse( KarbonPart* part )
	: VShapeTool( part )
{
	// create config dialog:
	m_dialog = new VEllipseDlg();
	m_dialog->setWidth( 100.0 );
	m_dialog->setHeight( 100.0 );
}

VCToolEllipse::~VCToolEllipse()
{
	delete( m_dialog );
}

VCToolEllipse*
VCToolEllipse::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolEllipse( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VCToolEllipse::drawTemporaryObject(
	KarbonView* view, const KoPoint& p, double d1, double d2 )
{
	VPainter *painter = view->painterFactory()->editpainter();

	VEllipseCmd* cmd =
		new VEllipseCmd( &part()->document(), p.x(), p.y(), p.x() + d1, p.y() + d2 );

	VObject* path = cmd->createPath();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

VCommand*
VCToolEllipse::createCmd( double x, double y, double d1, double d2 )
{
	if( d1 <= 1.0 && d2 <= 1.0 )
	{
		if ( m_dialog->exec() )
			return
				new VEllipseCmd( &part()->document(),
					x, y,
					x + m_dialog->width(),
					y + m_dialog->height() );
		else
			return 0L;
	}
	else
		return
			new VEllipseCmd( &part()->document(),
				x, y,
				x + d1,
				y + d2 );
}
