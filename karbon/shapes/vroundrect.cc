/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include <klocale.h>

#include "vroundrect.h"


VRoundRect::VRoundRect( VObject* parent,
		const KoPoint& topLeft, const KoPoint& bottomRight, double edgeRadius )
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

	if( edgeRadius < 0.0 )
		edgeRadius = 0.0;

	// catch case, when radius is larger than width or height:
	double minimum;

	if( edgeRadius  >
		( minimum =
			QMIN( ( br.x() - tl.x() ), ( tl.y() - br.y() ) ) * 0.5 ) )
	{
 		edgeRadius = minimum;
	}


	moveTo( KoPoint( tl.x(), tl.y() - edgeRadius ) );
	arcTo(
		KoPoint( tl.x(), tl.y() ),
		KoPoint( tl.x() + edgeRadius, tl.y() ), edgeRadius );
	arcTo(
		KoPoint( br.x(), tl.y() ),
		KoPoint( br.x(), tl.y() - edgeRadius ), edgeRadius );
	arcTo(
		KoPoint( br.x(), br.y() ),
		KoPoint( br.x() - edgeRadius, br.y() ), edgeRadius );
	arcTo(
		KoPoint( tl.x(), br.y() ),
		KoPoint( tl.x(), br.y() + edgeRadius ), edgeRadius );
	close();
}

