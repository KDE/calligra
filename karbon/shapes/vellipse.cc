/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include "vellipse.h"


VEllipse::VEllipse( VObject* parent,
		const KoPoint& topLeft, const KoPoint& bottomRight )
	: VPath( parent )
{
	KoPoint tl;
	KoPoint br;

	// Make sure that topLeft is really top left and bottomRight is bottom right:
	if ( topLeft.x() < bottomRight.x() )
	{
		tl.setX( topLeft.x() );
		br.setX( bottomRight.x() );
	}
	else
	{
		tl.setX( bottomRight.x() );
		br.setX( topLeft.x() );
	}

	if ( topLeft.y() > bottomRight.y() )
	{
		tl.setY( topLeft.y() );
		br.setY( bottomRight.y() );
	}
	else
	{
		tl.setY( bottomRight.y() );
		br.setY( topLeft.y() );
	}


	// First create (half-)unity circle around origin (0,0):
	moveTo( KoPoint( -0.5, 0.0 ) );
	arcTo( KoPoint(-0.5,  0.5 ), KoPoint( 0.0,  0.5 ), 0.5 );
	arcTo( KoPoint( 0.5,  0.5 ), KoPoint( 0.5,  0.0 ), 0.5 );
	arcTo( KoPoint( 0.5, -0.5 ), KoPoint( 0.0, -0.5 ), 0.5 );
	arcTo( KoPoint(-0.5, -0.5 ), KoPoint(-0.5,  0.0 ), 0.5 );
	close();

	double w = br.x() - tl.x();
	double h = tl.y() - br.y();

	// Translate and scale:
	QWMatrix m;
	m.translate( tl.x() + w * 0.5, br.y() + h * 0.5 );
	m.scale( w, h );
	transform( m );
}

