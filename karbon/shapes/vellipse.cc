/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include "vellipse.h"


VEllipse::VEllipse( VObject* parent,
		const KoPoint& topLeft, double width, double height )
	: VPath( parent )
{
	// Create (half-)unity circle with topLeft at (0|0):
	moveTo( KoPoint( 0.0, -0.5 ) );
	arcTo( KoPoint( 0.0, -1.0 ), KoPoint( 0.5, -1.0 ), 0.5 );
	arcTo( KoPoint( 1.0, -1.0 ), KoPoint( 1.0, -0.5 ), 0.5 );
	arcTo( KoPoint( 1.0,  0.0 ), KoPoint( 0.5,  0.0 ), 0.5 );
	arcTo( KoPoint( 0.0,  0.0 ), KoPoint( 0.0, -0.5 ), 0.5 );
	close();

	// Translate and scale:
	QWMatrix m;
	m.translate( topLeft.x(), topLeft.y() );
	m.scale( width, height );
	transform( m );
}

