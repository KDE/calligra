/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qwmatrix.h>

#include <klocale.h>

#include "vellipsecmd.h"
#include "vpath.h"


VEllipseCmd::VEllipseCmd( VDocument *doc,
		const double tlX, const double tlY,
		const double brX, const double brY )
	: VShapeCmd( doc, i18n( "Insert Ellipse" ) )
{
	// make sure that tl is really top-left and br is bottom-right:
	if ( tlX < brX )
	{
		m_tlX = tlX;
		m_brX = brX;
	}
	else
	{
		m_tlX = brX;
		m_brX = tlX;
	}
	if ( tlY > brY )
	{
		m_tlY = tlY;
		m_brY = brY;
	}
	else
	{
		m_tlY = brY;
		m_brY = tlY;
	}
}

VObject*
VEllipseCmd::createPath()
{
	// first create (half-)unity circle around origin (0,0):
	VPath* path = new VPath();
	path->moveTo( KoPoint( -0.5, 0.0 ) );
	path->arcTo( KoPoint(-0.5,  0.5 ), KoPoint( 0.0,  0.5 ), 0.5 );
	path->arcTo( KoPoint( 0.5,  0.5 ), KoPoint( 0.5,  0.0 ), 0.5 );
	path->arcTo( KoPoint( 0.5, -0.5 ), KoPoint( 0.0, -0.5 ), 0.5 );
	path->arcTo( KoPoint(-0.5, -0.5 ), KoPoint(-0.5,  0.0 ), 0.5 );
	path->close();

	double w = m_brX - m_tlX;
	double h = m_tlY - m_brY;

	// translate path and scale:
	QWMatrix m;
	m.translate( m_tlX + w*0.5, m_brY + h*0.5 );
	m.scale( w, h );
	path->transform( m );

	return path;
}

